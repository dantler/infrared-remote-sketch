/* main.ino - Infrared bit banger on Arduino101
 *
 * Copyright (C) 2016 David Antler
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 */
#include <CurieBLE.h>
#include <LircRemote101.h>

#define _countof(_Array) (sizeof(_Array) / sizeof(_Array[0]))

const unsigned short  kanto_yu5_power[] = {
 9023,    4565,     533,     597,     490,     618,
  490,     618,     490,     618,     490,     618,
  490,     618,     490,     618,     490,     618,
  490,    1727,     533,    1727,     533,    1749,
  511,    1727,     511,    1727,     533,    1727,
  533,    1727,     533,    1727,     533,     597,
  490,     618,     490,    1749,     511,    1749,
  511,    1727,     533,     618,     490,     618,
  490,     618,     490,    1727,     533,    1727,
  511,     597,     490,     618,     490,     618,
  490,    1727,     533,    1727,     533,    1706,
  533
};

const unsigned short projector_POWER_ON[] = {
  8298,    4245,     469,     554,     469,     554,
   469,     554,     469,     554,     469,    1599,
   469,    1599,     469,    1599,     469,    1599,
   469,    4223,     469,    1599,     469,     554,
   469,     554,     469,     554,     469,     554,
   447,     575,     447,    1599,     469,     554,
   447
};

const unsigned short projector_POWER_OFF[] = {
  8298,    4245,     469,     554,     469,     554,
   469,     575,     447,     575,     447,    1599,
   469,    1599,     469,    1599,     469,    1599,
   469,    4223,     469,     554,     469,    1599,
   490,     554,     469,     554,     469,     554,
   447,     554,     469,    1599,     469,     554,
   469
};

enum AirconButton {
  ON_OFF = 0,
  FAN_SPEED,
  MINUS_TEMP_TIME,
  PLUS_TEMP_TIME,
  TIMER,
  MODE
} AirconButton;

// IMPORTANT: Must declare fields IN ORDER or this will fail with 
// an error saying "non-trivial designated initializers not supported".

const LircRemote fedders_aircon = {
  .descriptions = (LRD_HEADER | LRD_ONE | LRD_ZERO | LRD_PTRAIL | LRD_REPEAT | LRD_GAP | LRD_TOGBITMASK ),
  .bits         = 32,
  .header = {8975, 4557},
  .one    = {505, 1723},
  .zero   = {505, 604},
  .ptrail = 512,
  .repeat = {8975, 2299},
  .toggle_bit_mask = 0,
  .button_codes = {
              //ON_OFF                   
              0x04FB40BF,
              //FAN_SPEED
              0x04FB58A7,
              //MINUS_TEMP_TIME
              0x04FB8877,
              //PLUS_TEMP_TIME 
              0x04FB50AF,
              //TIMER
              0x04FB9867,
              //MODE
              0x04FB906F
  }
};

const int IRLED_PIN = 3; // GPIO used for IR LED
extern BLEPeripheral blePeripheral;
extern BLECharCharacteristic switchChar;
extern BLECharCharacteristic airconIsOn;
extern BLEUnsignedLongCharacteristic temperature;

const int THERMRISTOR_ANALOG_PIN = A0;  // ADC used for thermristor

void setup() {
  // put your setup code here, to run once:
  pinMode(13, OUTPUT);
  Serial.begin(9600);
  Serial.print("Starting...");
  initBluetooth();
}

void toggleAircon() {
  airconIsOn.setValue((airconIsOn.value() == 0) ? 1 : 0);
}

#include <math.h>

void doReadThermals() {
  int sensorValue = 0;
  int loopMax = 100;
  for(int i = 0; i < loopMax; i++) {
    sensorValue += analogRead(THERMRISTOR_ANALOG_PIN);
  }
  float voltage = sensorValue * (3.3) / (loopMax * 1023.0);

  float degreesC, degreesF;

  degreesC = (voltage - 0.5) * 100.0;
  degreesF = degreesC * (9.0/5.0) + 32.0;
  temperature.setValue((int)roundf(degreesF*10));
}

void loop() {
  // put your main code here, to run repeatedly:
  blePeripheral.poll();
}

void switchCharacteristicWritten(BLEHelper& central, BLECharacteristic& characteristic) {
   // central wrote new value to characteristic, update LED
   Serial.print("Characteristic event, written: \n");
   digitalWrite(13, HIGH);
   switch(switchChar.value()) {
    case 0:
         toggleAircon();
         sendRemoteCode(&fedders_aircon, ON_OFF, IRLED_PIN);
         break;
         
    case 1:
         sendRawCode(kanto_yu5_power, 
                 _countof(kanto_yu5_power), 
                 IRLED_PIN);
         break;
         
    case 2:
         for(int i = 0; i < 3; i++) {
           sendRawCode(projector_POWER_ON, 
                   _countof(projector_POWER_ON), 
                   IRLED_PIN);
           delayMicroseconds(68780);
         }
         break;
         
    case 3:
         for(int i = 0; i < 3; i++) {
           sendRawCode(projector_POWER_OFF, 
                   _countof(projector_POWER_OFF), 
                   IRLED_PIN);
           delayMicroseconds(68889);
         }
         break;
         
    default:
         Serial.print("Received unknown code!");
         break;
   }
   digitalWrite(13, LOW);
}

