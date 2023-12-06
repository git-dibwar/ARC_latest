#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;
bool relayState = false;

#define SERVICE_UUID        "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define BUTTON_PIN          2  // Change this to the GPIO pin connected to the press switch
#define RELAY_PIN           4  // Change this to the GPIO pin connected to the relay module

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      // Handle connections
      deviceConnected = true;
    }

    void onDisconnect(BLEServer* pServer) {
      // Handle disconnections
      deviceConnected = false;
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

  BLEService *pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      //                      BLECharacteristic::PROPERTY_READ |
                      //                      BLECharacteristic::PROPERTY_WRITE |
                      BLECharacteristic::PROPERTY_NOTIFY //|
                      //                      BLECharacteristic::PROPERTY_INDICATE
                    );

  pCharacteristic->addDescriptor(new BLE2902());

  pService->start();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
  //  Serial.println("Waiting a client connection to notify...");
  if (deviceConnected == true) {
    Serial.println("Device connected");
  }
  else {
    Serial.println("Waiting a client connection to notify...");
  }
}

void loop() {
  if (digitalRead(BUTTON_PIN) == LOW) {
    //    if (!relayState) {
    // Button pressed, notify phone and turn on relay
    const char* message = "Danger";
    pCharacteristic->setValue(message);
    pCharacteristic->notify();
    delay(10);
    digitalWrite(RELAY_PIN, HIGH);
    Serial.println("It's ON");
    relayState = true;
    //  }
  } else {
    //    if (relayState) {
    // Button released, turn off relay
    const char* message = "Not Danger";
    pCharacteristic->setValue(message);
    pCharacteristic->notify();
    delay(10);
    digitalWrite(RELAY_PIN, LOW);
    Serial.println("It's OFF");
    relayState = false;
  }
  //}

  delay(100);
}
