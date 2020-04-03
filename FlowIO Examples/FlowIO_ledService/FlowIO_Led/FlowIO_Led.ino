#include <bluefruit.h>
#include <FlowIO.h>

#define DEVICE_NAME "nrf52 LED Control"// Device Name: Maximum 30 bytes

FlowIO flowio;

BLEService ledService;
BLECharacteristic chrLedStates;
uint8_t ledStates[2];

void setup() {
  Serial.begin(9600);
  flowio = FlowIO(GENERAL);
  Bluefruit.begin();
  Bluefruit.setName(DEVICE_NAME);
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);

  createLedService(); //This is defined in the file "powerService.ino"
  startAdvertising();
}

void startAdvertising(void) {
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.ScanResponse.addName();
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.addService(ledService); //advertise service uuic

  Bluefruit.Advertising.setInterval(32, 244); // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);   // number of seconds in fast mode
  
  Bluefruit.Advertising.start(0);
}

void loop() {
}

void connect_callback(uint16_t conn_handle){ // callback invoked when central connects  
  //Whenver a central device writes to the nrf52 to enable notifications, the RED led will flash once.
  //This is a default behavior of the feather board, which seems like it cannot be changed. 
  //This happens with any sketch on characteristic that supports notifications. 
  //Thus, if you try having the RED led turn on upon connect, it will turn off right away as soon as the 
      //connected device gells the nrf52 feather that it subscribes to notifications. Hence, you should
      //always have the red LED be off upon connect to avoid surprises.
  ledStates[0] = 0x00;
  ledStates[1] = 0x01;
  flowio.redLED(ledStates[0]);
  flowio.blueLED(ledStates[1]);
  chrLedStates.notify(ledStates,2);
}
void disconnect_callback(uint16_t conn_handle, uint8_t reason){
  ledStates[0] = 0x00;
  ledStates[1] = 0x00;
  chrLedStates.write(ledStates,sizeof(ledStates));
  flowio.redLED(ledStates[0]);
}