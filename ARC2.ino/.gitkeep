#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool relayState = false;

#define SERVICE_UUID        "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define BUTTON_PIN          2  // Change this to the GPIO pin connected to the press switch
#define RELAY_PIN           4  // Change this to the GPIO pin connected to the relay module

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    // Handle connections
    deviceConnected = true;
  }

  void onDisconnect(BLEServer* pServer) {
    // Handle disconnections
    deviceConnected = false;

    // Start advertising again for reconnection
    pServer->startAdvertising();
    Serial.println("Advertising started for reconnection...");
  }
};

void setup() {
  Serial.begin(115200);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  BLEDevice::init("YourESP32");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService* pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_NOTIFY
  );

  pCharacteristic->addDescriptor(new BLE2902());

  pService->start();

  BLEAdvertising* pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
  Serial.println("Waiting for a client connection to notify...");
}

void loop() {
  if (deviceConnected) {
    // Notify data to the mobile device using byteValue
    uint8_t dataToSend[] = { relayState ? 1 : 0 };
    pCharacteristic->setValue(dataToSend, sizeof(dataToSend));
    pCharacteristic->notify();
    delay(1000); // Adjust the delay according to your requirements
  }

  if (digitalRead(BUTTON_PIN) == LOW) {
    // Button pressed, notify phone and turn on relay
    uint8_t dataToSend[] = { 1 };
    pCharacteristic->setValue(dataToSend, sizeof(dataToSend));
    pCharacteristic->notify();
    delay(10);
    digitalWrite(RELAY_PIN, HIGH);
    Serial.println("It's ON");
    relayState = true;
  } else {
    // Button released, turn off relay
    uint8_t dataToSend[] = { 0 };
    pCharacteristic->setValue(dataToSend, sizeof(dataToSend));
    pCharacteristic->notify();
    delay(10);
    digitalWrite(RELAY_PIN, LOW);
    Serial.println("It's OFF");
    relayState = false;
  }

  delay(100);
}
