/* ir_ble.ino - Infrared bit banger on Arduino101
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

int GPIO_PIN = 3; // Used for IR LED
extern BLEPeripheral blePeripheral;
extern BLECharCharacteristic switchChar;
#define MINUS_VALUE  10

void bruteForceTheCode()
{
  // Note: We need to have a static array so that _countof actually works.
  static unsigned short arrayCopy[_countof(projector_POWER_ON)];

  // Copy from target into source.
  for(unsigned int i = 0; i < _countof(arrayCopy); i++) {
    arrayCopy[i] = projector_POWER_ON[i] - MINUS_VALUE;
  }
  
  for(unsigned short k = 0; k < MINUS_VALUE*2; k++) {
    //unsigned long j = 68890;
    for(long j = 67600; j < 69000; j += 10) {
       for(int repeat = 0; repeat < 10; repeat++) {

         sendRawCode(arrayCopy, 
                     _countof(arrayCopy), 
                     GPIO_PIN);
         delayMicroseconds(j);

       }

       
       Serial.print("Just ran: j = ");
       Serial.print(j, DEC);
       Serial.print(", k = ");
       Serial.println(k, DEC);
       
       delay(100);
       //digitalWrite(13, HIGH);
    }

    for(unsigned int i = 0; i < _countof(arrayCopy); i++) {
      arrayCopy[i] += 1;
    }
  }
  // Light up the LED when done brute forcing
  digitalWrite(13, HIGH);
}

void setup() {
  // put your setup code here, to run once:
  pinMode(13, OUTPUT);
  Serial.begin(9600);
  Serial.print("Starting...");
  //bruteForceTheCode();
  initBluetooth();
}

void loop() {
  // put your main code here, to run repeatedly:
  blePeripheral.poll();
}

void switchCharacteristicWritten(BLECentral& central, BLECharacteristic& characteristic) {
   // central wrote new value to characteristic, update LED
   Serial.print("Characteristic event, written: \n");
   digitalWrite(13, HIGH);
   switch(switchChar.value()) {
    case 0:
         sendRemoteCode(&fedders_aircon, ON_OFF, GPIO_PIN);
         Serial.print("Aircon sent\n");
         break;
         
    case 1:
         sendRawCode(kanto_yu5_power, 
                 _countof(kanto_yu5_power), 
                 GPIO_PIN);
         Serial.print("Kanto sent\n");
         break;
         
    case 2:
         for(int i = 0; i < 3; i++) {
           sendRawCode(projector_POWER_ON, 
                   _countof(projector_POWER_ON), 
                   GPIO_PIN);
           delayMicroseconds(68780);
         }
         break;
         
    case 3:
         for(int i = 0; i < 3; i++) {
           sendRawCode(projector_POWER_OFF, 
                   _countof(projector_POWER_OFF), 
                   GPIO_PIN);
           delayMicroseconds(68889);
         }
         break;
         
    default:
         Serial.print("Received unknown code!");
         break;
   }
   digitalWrite(13, LOW);
}

