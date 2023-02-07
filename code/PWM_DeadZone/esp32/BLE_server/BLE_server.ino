#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>


#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
String  v;
String  temp;
BLEServer* pServer;
BLEService* pService;
BLECharacteristic* pCharacteristic;
std::string Msg = "Hello World!";


void bluetooth();

class MyCharacteristicCallback : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic* pCharacteristic)
    {
      const char* value = pCharacteristic->getValue().c_str();
      String str = value;
      v = value;
      if (true)
      {
        Serial.print(" - OnWrite: ");
        Serial.println(str);
        temp = v;
        String reply = "ToClient: Server read: " + v ;
        const char* charReply = reply.c_str();
        pCharacteristic->setValue(charReply);
        Serial.println(charReply);
      }
      else
      {
        Serial.print(" - Unexpected data: ");
        Serial.println(str);
      }
    }

    void onNotify(BLECharacteristic* pCharacteristic)
    {
      Serial.print(" - OnNotify: ");
      Serial.println(pCharacteristic->getValue().c_str());
    }
};

class MyBLEServerCallbacks : public BLEServerCallbacks
{
    void onConnect(BLEServer* pServer)
    {
      Serial.println(" - ----------");
      Serial.println(" - Connected");
      Serial.println(" - ----------");
    }

    void onDisconnect(BLEServer* pServer)
    {
      Serial.println(" - ----------");
      Serial.println(" - Disconnected");
      Serial.println(" - ----------");
    }
};
void bluetooth() {
  Serial.begin(115200);
  Serial.println("Starting BLE server work!");
  delay(10000);

  BLEDevice::init("Long name works now");
  pServer = BLEDevice::createServer();
  pService = pServer->createService(SERVICE_UUID);
  pServer->setCallbacks(new MyBLEServerCallbacks());
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE
                    );
  pCharacteristic->setValue(Msg);
  pCharacteristic->setCallbacks(new MyCharacteristicCallback());
  pService->start();
  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");

}
void setup()
{
  bluetooth();

}

void loop()
{
  int i = 0;
  while(1){
  prinf("waiting for %d seconds\n\r");
  i=i+5;
  }
  delay(5000);
}
