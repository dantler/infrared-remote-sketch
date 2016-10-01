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
BLEDescriptor switchCharLabel("19B10002-E8F2-537C-4F6C-D104768BC101", "SendRemoteCode");

void initBluetooth()
{
    // set the local name peripheral advertises
   blePeripheral.setLocalName("IRLED");
   // set the UUID for the service this peripheral advertises
   blePeripheral.setAdvertisedServiceUuid(ledService.uuid());

   blePeripheral.setDeviceName("IR Controller");

   // add service and characteristic
   blePeripheral.addAttribute(ledService);
   blePeripheral.addAttribute(switchChar);
   blePeripheral.addAttribute(switchCharLabel);

   // assign event handlers for connected, disconnected to peripheral
   blePeripheral.setEventHandler(BLEConnected, blePeripheralConnectHandler);
   blePeripheral.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);

   // assign event handlers for characteristic
   switchChar.setEventHandler(BLEWritten, switchCharacteristicWritten);
   // set an initial value for the characteristic
   switchChar.setValue(0);

   // advertise the service
   blePeripheral.begin();
}

void blePeripheralConnectHandler(BLECentral& central) {
   // central connected event handler
   Serial.print("Connected event, central: ");
   Serial.println(central.address());
}

void blePeripheralDisconnectHandler(BLECentral& central) {
   // central disconnected event handler
   Serial.print("Disconnected event, central: ");
   Serial.println(central.address());
}

