#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <string.h>

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
String  v ;
#define choice 1 //choice 1 = car choice 2 = gun
const char *left = "left";
const char *right = "right";
const char *forward = "forward";
const char *backward = "backward";
const char *shoot = "shoot";
const char *notyet = "notyet";
BLEServer* pServer;
BLEService* pService;
BLECharacteristic* pCharacteristic;
std::string Msg = "Hello World!";


void bluetooth();
void docommand();

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
      ESP.restart();
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
if (choice == 1){
  const char *tmp1 = v.c_str();
  while (1) {

    if ((strcmp(tmp1 , left)) == 0) {
      Serial.println("left!\n");
      digitalWrite(16 , HIGH);
      delay(1000);
      digitalWrite(16, LOW);
    }
    else if ((strcmp(tmp1 , right)) == 0) {
      Serial.println("right!\n");
      digitalWrite(17, HIGH);
      delay(1000);
      digitalWrite(17, LOW);
    }
    else if ((strcmp(tmp1 , forward)) == 0) {
      Serial.println("forward!\n");
      digitalWrite(18, HIGH);
      delay(1000);
      digitalWrite(18, LOW);
    }
    else if ((strcmp(tmp1 , backward)) == 0) {
      Serial.println("backward!\n");
      digitalWrite(19, HIGH);
      delay(1000);
      digitalWrite(19, LOW);
    }
    else if ((strcmp(tmp1 , notyet)) == 0) {
      Serial.println("haven't receive any message!\n");
      delay(1000);
    }
    else {
      printf("do nothing\n");
      delay(5000);
    }
    tmp1 = "notyet";
  }}
  /*
if(choice == 2){
  const char *tmp1 = v.c_str();
  while (1) {
    if ((strcmp(tmp1 , left)) == 0) {
      Serial.println("left!\n");
      digitalWrite(16 , HIGH);
      delay(1000);
      digitalWrite(16, LOW);
    }
    else if ((strcmp(tmp1 , right)) == 0) {
      Serial.println("right!\n");
      digitalWrite(17, HIGH);
      delay(1000);
      digitalWrite(17, LOW);
    }
    else if ((strcmp(tmp1 , forward)) == 0) {
      Serial.println("up!\n");
      digitalWrite(18, HIGH);
      delay(1000);
      digitalWrite(18, LOW);
    }
    else if ((strcmp(tmp1 , down)) == 0) {
      Serial.println("backward!\n");
      digitalWrite(19, HIGH);
      delay(1000);
      digitalWrite(19, LOW);
    }
    else if ((strcmp(tmp1 , shoot)) == 0) {
      Serial.println("shoot!\n");
      digitalWrite(21, HIGH);
      delay(1000);
      digitalWrite(21, LOW);
    }
    else if ((strcmp(tmp1 , notyet)) == 0) {
      Serial.println("haven't receive any message!\n");
      delay(1000);
    }
    else {
      printf("do nothing\n");
      delay(5000);
    }
    tmp1 = "notyet";
  }}
  */
}
