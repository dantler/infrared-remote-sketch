/* ir_ble.ino - Infrared bit banger on Arduino101
 *
 * Copyright (C) 2016 David Antler
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 */

#include <CurieBLE.h>

BLEPeripheral blePeripheral; // create peripheral instance

BLEService ledService("19B10000-E8F2-537C-4F6C-D104768BC101"); // create service

// create switch characteristic and allow remote device to read and write
BLECharCharacteristic switchChar("19B10001-E8F2-537C-4F6C-D104768BC101", BLERead | BLEWrite);
//BLEDescriptor switchCharLabel("19B10002-E8F2-537C-4F6C-D104768BC101", "SendRemoteCode");

//BLEService temperatureService("19B10003-E8F2-537C-4F6C-D104768BC101");
BLEUnsignedLongCharacteristic temperature("19B10002-E8F2-537C-4F6C-D104768BC101", BLERead);
//BLEDescriptor temperatureLabel("19B10005-E8F2-537C-4F6C-D104768BC101", "TemperatureReading");

BLECharCharacteristic airconIsOn("19B10003-E8F2-537C-4F6C-D104768BC101", BLERead);

// Note: BLE_STATUS_SUCCESS = 0
const BleStatus BleStatusSuccess = BLE_STATUS_SUCCESS;

#define CHECK_BLE_RETURN(Input) { \
if (BleStatusSuccess != (Input)) { \
    Serial.print("Failed!! Error = "); Serial.println(Input); \
} \
}

void initBluetooth()
{
    // set the local name peripheral advertises
   blePeripheral.setLocalName("IRLED");
   // set the UUID for the service this peripheral advertises
   blePeripheral.setAdvertisedServiceUuid(ledService.uuid());

   blePeripheral.setDeviceName("IR Controller");
   //blePeripheral.setLocalName("Temp monitor and IR");

   // add service and characteristic
   CHECK_BLE_RETURN(blePeripheral.addAttribute(ledService));
   CHECK_BLE_RETURN(blePeripheral.addAttribute(switchChar));
   //CHECK_BLE_RETURN(blePeripheral.addAttribute(switchCharLabel));
   //CHECK_BLE_RETURN(blePeripheral.addAttribute(temperatureService));
   CHECK_BLE_RETURN(blePeripheral.addAttribute(temperature));
   //CHECK_BLE_RETURN(blePeripheral.addAttribute(temperatureLabel));
   CHECK_BLE_RETURN(blePeripheral.addAttribute(airconIsOn));

   // assign event handlers for connected, disconnected to peripheral
   blePeripheral.setEventHandler(BLEConnected, blePeripheralConnectHandler);
   blePeripheral.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);

   // assign event handlers for characteristic
   switchChar.setEventHandler(BLEWritten, switchCharacteristicWritten);
   // set an initial value for the characteristic
   switchChar.setValue(0);
   airconIsOn.setValue(0);

   // Lie and set an initial value for this one too...
   temperature.setValue(300);

   // advertise the service
   blePeripheral.begin();
}

void blePeripheralConnectHandler(BLEHelper& central) {
   // central connected event handler
   Serial.print("Connected event, central: ");
   Serial.println(central.address());
}

void blePeripheralDisconnectHandler(BLEHelper& central) {
   // central disconnected event handler
   Serial.print("Disconnected event, central: ");
   Serial.println(central.address());
}

