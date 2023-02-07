#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <string.h>

#define SERVICE_UUID        "5b1933aa-1f66-4a84-a726-f839e12c5f8f"
#define CHARACTERISTIC_UUID "2cbc54c4-2bc1-4a9a-9fb8-17bea95bfd16"
#define choice 1 //choice 1 = car choice 2 = gun

#define FL_PWM_PIN 2
#define FL_IN1_PIN 4
#define FL_IN2_PIN 16
#define RL_PWM_PIN 17
#define RL_IN1_PIN 21
#define RL_IN2_PIN 22
#define FR_PWM_PIN 14
#define FR_IN1_PIN 27
#define FR_IN2_PIN 26
#define RR_PWM_PIN 25
#define RR_IN1_PIN 33
#define RR_IN2_PIN 32

#define FR_CHANNEL 1
#define FL_CHANNEL 2
#define RR_CHANNEL 3
#define RL_CHANNEL 4
int outputPower = 0;
int turnPower = 0;
int forwardStatus = 1;
int feq = 1000;
int dutyCycle = 210;
int continueOutput = false;
int stoped = true;
int incomingByte = 0;
String  v ;



const char *Left = "left";
const char *Right = "right";
const char *Forward = "forward";
const char *Backward = "backward";
const char *shoot = "shoot";
const char *notyet = "notyet";
BLEServer* pServer;
BLEService* pService;
BLECharacteristic* pCharacteristic;
std::string Msg = "Hello World!";

void stopCar() {
  ledcWrite(1, 0);
  ledcWrite(2, 0);
  ledcWrite(3, 0);
  ledcWrite(4, 0);
  stoped = true;
}

void enPower() {
  //int p1 = map(outputPower, 0, 1023, 0, 255);
  int p1 = map(outputPower, 0, 512, 0, 128);
  ledcWrite(1, p1);
  ledcWrite(2, p1);
  ledcWrite(3, p1);
  ledcWrite(4, p1);
  stoped = false;
}

void forward() {
  forwardStatus = 1;
  digitalWrite(FR_IN1_PIN, HIGH);
  digitalWrite(FR_IN2_PIN, LOW);
  digitalWrite(FL_IN1_PIN, HIGH);
  digitalWrite(FL_IN2_PIN, LOW);
  digitalWrite(RR_IN1_PIN, HIGH);
  digitalWrite(RR_IN2_PIN, LOW);
  digitalWrite(RL_IN1_PIN, HIGH);
  digitalWrite(RL_IN2_PIN, LOW);
}
void shutdownall() {
  //forwardStatus = 1;
  digitalWrite(FR_IN1_PIN, LOW);
  digitalWrite(FR_IN2_PIN, LOW);
  digitalWrite(FL_IN1_PIN, LOW);
  digitalWrite(FL_IN2_PIN, LOW);
  digitalWrite(RR_IN1_PIN, LOW);
  digitalWrite(RR_IN2_PIN, LOW);
  digitalWrite(RL_IN1_PIN, LOW);
  digitalWrite(RL_IN2_PIN, LOW);
}

void goBack() {
  forwardStatus = 0;
  digitalWrite(FR_IN1_PIN, LOW);
  digitalWrite(FR_IN2_PIN, HIGH);
  digitalWrite(FL_IN1_PIN, LOW);
  digitalWrite(FL_IN2_PIN, HIGH);
  digitalWrite(RR_IN1_PIN, LOW);
  digitalWrite(RR_IN2_PIN, HIGH);
  digitalWrite(RL_IN1_PIN, LOW);
  digitalWrite(RL_IN2_PIN, HIGH);
}

void spinRight() {
  digitalWrite(FR_IN1_PIN, LOW);
  digitalWrite(FR_IN2_PIN, HIGH);
  digitalWrite(RR_IN1_PIN, LOW);
  digitalWrite(RR_IN2_PIN, HIGH);
}

void spinLeft() {
  digitalWrite(FL_IN1_PIN, LOW);
  digitalWrite(FL_IN2_PIN, HIGH);
  digitalWrite(RL_IN1_PIN, LOW);
  digitalWrite(RL_IN2_PIN, HIGH);
}

void foward() {
  Serial.print("forward\n");
  continueOutput = true;
  forward();
  enPower();
  delay(100);
  continueOutput = false;
  stopCar();
  shutdownall();
  delay(100);
}
void backward() {
  Serial.print("goback\n");
  continueOutput = true;
  goBack();
  enPower();
  delay(100);
  continueOutput = false;
  stopCar();
  shutdownall();
  delay(100);
}
void spinright() {
  Serial.println("spin right\n");
  forward(); //先回到前進狀態
  spinRight();
  enPower();
  delay(100);
  stopCar();
  shutdownall();
  delay(100);
}
void spinleft() {
  Serial.println("\nspin left\n");
  forward(); //先回到前進狀態
  spinLeft();
  enPower();
  delay(100);
  stopCar();
  shutdownall();
  delay(100);

}

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
  Serial.println("Starting BLE server work!");

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
  Serial.begin(115200);
  bluetooth();
  printf("setup\n\n");
  ledcAttachPin(FR_PWM_PIN, FR_CHANNEL); // assign RGB led pins to channels
  ledcAttachPin(FL_PWM_PIN, FL_CHANNEL);
  ledcAttachPin(RR_PWM_PIN, RR_CHANNEL);
  ledcAttachPin(RL_PWM_PIN, RL_CHANNEL);
  ledcSetup(FR_CHANNEL, feq, 8); // 12 kHz PWM, 8-bit resolution
  ledcSetup(FL_CHANNEL, feq, 8);
  ledcSetup(RR_CHANNEL, feq, 8);
  ledcSetup(RL_CHANNEL, feq, 8);
  pinMode(FR_IN1_PIN, OUTPUT);
  pinMode(FR_IN2_PIN, OUTPUT);
  pinMode(FL_IN1_PIN, OUTPUT);
  pinMode(FL_IN2_PIN, OUTPUT);
  pinMode(RR_IN1_PIN, OUTPUT);
  pinMode(RR_IN2_PIN, OUTPUT);
  pinMode(RL_IN1_PIN, OUTPUT);
  pinMode(RL_IN2_PIN, OUTPUT);
}

void loop()
{
    if(1){
    
    const char *tmp1 = v.c_str();
    printf("\ntmp1 = %s", tmp1);
    
      if ((strcmp(tmp1 , Left))==0) {
        Serial.println("left!\n");
        spinleft();
      }
      else if ((strcmp(tmp1 , Right))==0) {
        Serial.println("right!\n");
        spinright();
      }
      else if ((strcmp(tmp1 , Forward))==0) {
        Serial.println("here's forward!\n");
        foward();
      }
      else if ((strcmp(tmp1 , Backward))==0) {
        Serial.println("backward!\n");
        backward();
      }
      else if ((strcmp(tmp1 , notyet)) == 0) {
        Serial.println("haven't receive any message!\n");
      }
      else {
        printf("do nothing\n");
      }
      tmp1 = "not yet";
      delay(500);
}}
