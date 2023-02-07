#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <esp_bt_main.h>
#include <dummy.h>

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

static BLEUUID serviceUUID("5b1933aa-1f66-4a84-a726-f839e12c5f8f");
static BLEUUID    charUUID("2cbc54c4-2bc1-4a9a-9fb8-17bea95bfd16");
int left = 10000, right = 10000, up = 10000, down = 10000, shoot = 10000;
int box1=1,box2=0;
static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;
int i = 0;
int incomingByte = 0;
String oldData = "";
String strnew_value = "notyet";
String readValue;
String line_message;
char rate_char[15];


int outputPower = 0;
int turnPower = 0;
int forwardStatus = 1;
int feq = 1000;
int dutyCycle = 210;
int continueOutput = false;
int stoped = true;
static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify)
{
  Serial.print("Notify callback for characteristic ");
  Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
  Serial.print(" of data length ");
  Serial.println(length);
  Serial.print("data: ");
  Serial.println((char*)pData);
}
class MyClientCallback : public BLEClientCallbacks
{
    void onConnect(BLEClient* pclient)
    {
      Serial.println(" - ----------");
      Serial.println(" - onConnect");
      Serial.println(" - ----------");
    }

    void onDisconnect(BLEClient* pclient)
    {
      connected = false;
      Serial.println(" - ----------");
      Serial.println(" - onDisconnect");
      Serial.println(" - ----------");
      //ESP.restart();
    }
};

bool connectToServer()
{
  Serial.print("Forming a connection to ");
  Serial.println(myDevice->getAddress().toString().c_str());

  BLEClient* pClient = BLEDevice::createClient();
  Serial.println(" - Created client");

  pClient->setClientCallbacks(new MyClientCallback());

  pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
  Serial.println("----------------------");
  Serial.println(" - Connected to server");
  Serial.println("----------------------");

  BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
  if (pRemoteService == nullptr)
  {
    Serial.print("Failed to find our service UUID: ");
    Serial.println(serviceUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Found our service");


  pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
  if (pRemoteCharacteristic == nullptr)
  {
    Serial.print("Failed to find our characteristic UUID: ");
    Serial.println(charUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Found our characteristic");

  if (pRemoteCharacteristic->canRead())
  {
    std::string value = pRemoteCharacteristic->readValue();
    Serial.print("The characteristic value was: ");
    Serial.println(value.c_str());
  }

  if (pRemoteCharacteristic->canNotify())
    pRemoteCharacteristic->registerForNotify(notifyCallback);

  connected = true;
}

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
    /**
        Called for each advertising BLE server.
    */
    void onResult(BLEAdvertisedDevice advertisedDevice)
    {
      Serial.print("BLE Advertised Device found: ");
      Serial.println(advertisedDevice.toString().c_str());

      // We have found a device, let us now see if it contains the service we are looking for.
      printf("1\n");
      if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID))
      {
        printf("2\n");
        BLEDevice::getScan()->stop();
        myDevice = new BLEAdvertisedDevice(advertisedDevice);
        doConnect = true;
        doScan = true;
        printf("3\n");
      } // Found our server
    } // onResult
}; // MyAdvertisedDeviceCallbacks

static void Task_CheckDataFromServer(void* pvParameters)
{
  while (true)
  {
    if (connected)
    {
      readValue = pRemoteCharacteristic->readValue().c_str();
      if (readValue.startsWith("ToClient: ") && readValue != oldData)
      {
        oldData = readValue;
        String data = oldData.substring(10);
        Serial.println(" - From server: " + data);
      }
    }
    delay(10);
  }
}
void  ble_connect() {
  BLEDevice::init("BLE_Client");
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  //pBLEScan->start(5, false);
  pBLEScan->start(0);
  printf("4\n");
}
void stopCar() {
  ledcWrite(1, 0);
  ledcWrite(2, 0);
  ledcWrite(3, 0);
  ledcWrite(4, 0);
  stoped = true;
}

void enPower() {
  int p1 = map(outputPower, 0, 1023, 0, 255);
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

void setup()
{
  // Debug console
  Serial.begin(115200);
  printf("setup\n\n");
  Serial.println("Starting Arduino BLE Client application...");
  ble_connect();
  printf("5\n");
  if (doConnect == true)
  {
    if (connectToServer())
      Serial.println("We are now connected to the BLE Server.");
    else
      Serial.println("We have failed to connect to the server; there is nothin more we will do.");
    doConnect = false;
  }
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
  incomingByte = 0;
    
    incomingByte = Serial.read();
    Serial.print("I received: ");
    Serial.println(incomingByte, DEC);
    
 /*
    down = analogRead(25);
    right = analogRead(26);
    up = analogRead(32);
    left = analogRead(34);
*/
    
    if ((down < 2800)||(incomingByte == 50)) {
      Serial.print("down");
      strnew_value = "backward";
    }
    else if ((right < 2700)||(incomingByte == 54)) {
      Serial.print("right");
      strnew_value = "right";
    }
    else if ((up < 3000)||(incomingByte == 56)) {
      Serial.print("up");
      strnew_value = "forward";
    }
    else if ((left < 1700)||(incomingByte == 52)) {
      Serial.print("left");
      strnew_value = "left";
    }
    else {
      Serial.print("not yet");
      strnew_value = "not yet";
    }
    delay(500);
    Serial.println("");
 
  

    if (connected)
  {
    Serial.println("ToServer: \"" + strnew_value + "\"");
    Serial.println(String(strnew_value));
    pRemoteCharacteristic->writeValue(strnew_value.c_str(), strnew_value.length());
  }
  else if (doScan)
  {
    Serial.printf("Scanning server");
    BLEDevice::getScan()->start(0);
  }
  delay(1000);
  
 
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

/*
  //set output power level
  void fn3() {
  //outputPower = 1;
  //Serial.print("set power = ");
  //Serial.println(outputPower);
  if(continueOutput){
    enPower();
  }
  }

  //設定轉彎力道
  void fn1() {
  turnPower = 1;
  Serial.print("set turn power = ");
  Serial.println(turnPower);
  }
*/
/*
  void turnright() {
  int x ;
  Serial.println("=== turn right ===");
  if(x == 1){
    goRight();
    right();
  }
  else{
    if(forwardStatus==1){
      forward();
    }
    else
    {
      goBack();
    }
    if(continueOutput){
      enPower();
    }
    else
    {
      stopCar();
    }
  }
  }

  void turnleft() {
  //int x = 1;
  Serial.println("=== turn left ===");
  if(x == 1){
    goLeft();
    left();
  }else{
    if(forwardStatus==1){
      forward();
    }
    else
    {
      goBack();
    }
    if(continueOutput){
      enPower();
    }
    else
    {
      stopCar();
    }
  }
  }
*/
/*
void goRight() {
  //如果是前進狀態，停右前輪逆轉右後輪
  //如果是倒退狀態，停右後輪逆轉右前輪
  if (forwardStatus == 1) {
    digitalWrite(FR_IN1_PIN, HIGH);
    digitalWrite(FR_IN2_PIN, LOW);
    digitalWrite(FL_IN1_PIN, HIGH);
    digitalWrite(FL_IN2_PIN, LOW);
    digitalWrite(RR_IN1_PIN, LOW);
    digitalWrite(RR_IN2_PIN, HIGH);
    digitalWrite(RL_IN1_PIN, HIGH);
    digitalWrite(RL_IN2_PIN, LOW);
  } else {
    digitalWrite(FR_IN1_PIN, HIGH);
    digitalWrite(FR_IN2_PIN, LOW);
    digitalWrite(FL_IN1_PIN, LOW);
    digitalWrite(FL_IN2_PIN, HIGH);
    digitalWrite(RR_IN1_PIN, LOW);
    digitalWrite(RR_IN2_PIN, HIGH);
    digitalWrite(RL_IN1_PIN, LOW);
    digitalWrite(RL_IN2_PIN, HIGH);
  }
}

void goLeft() {
  //如果是前進狀態，停左前輪逆轉左後輪
  //如果是倒退狀態，停左後輪逆轉左前輪
  if (forwardStatus == 1) {
    digitalWrite(FR_IN1_PIN, HIGH);
    digitalWrite(FR_IN2_PIN, LOW);
    digitalWrite(FL_IN1_PIN, HIGH);
    digitalWrite(FL_IN2_PIN, LOW);
    digitalWrite(RR_IN1_PIN, HIGH);
    digitalWrite(RR_IN2_PIN, LOW);
    digitalWrite(RL_IN1_PIN, LOW);
    digitalWrite(RL_IN2_PIN, HIGH);
  } else {
    digitalWrite(FR_IN1_PIN, LOW);
    digitalWrite(FR_IN2_PIN, HIGH);
    digitalWrite(FL_IN1_PIN, HIGH);
    digitalWrite(FL_IN2_PIN, LOW);
    digitalWrite(RR_IN1_PIN, LOW);
    digitalWrite(RR_IN2_PIN, HIGH);
    digitalWrite(RL_IN1_PIN, LOW);
    digitalWrite(RL_IN2_PIN, HIGH);
  }
}*/
/*
 * 
void right() {
  int p1 = map(outputPower, 0, 1023, 0, 255);
  int p2 = map(turnPower, 0, 1023, 0, 255);
  if (forwardStatus == 1) {
    ledcWrite(FR_CHANNEL, 0);
    ledcWrite(RR_CHANNEL, p2);
    ledcWrite(FL_CHANNEL, p1);
    ledcWrite(RL_CHANNEL, p1);
  } else {
    ledcWrite(FR_CHANNEL, p2);
    ledcWrite(RR_CHANNEL, 0);
    ledcWrite(FL_CHANNEL, p1);
    ledcWrite(RL_CHANNEL, p1);
  }
  //如果是前進狀態，停右前輪
  //如果是倒退狀態，停右後輪
  // if(forwardStatus==1){
  //   ledcWrite(FR_CHANNEL,0);
  //   ledcWrite(FL_CHANNEL,p2);
  // }else{
  //   ledcWrite(FR_CHANNEL,p1);
  //   ledcWrite(FL_CHANNEL,p1);
  // }
  // if(forwardStatus==1){
  //   ledcWrite(RR_CHANNEL,p1);
  //   ledcWrite(RL_CHANNEL,p1);
  // }else{
  //   ledcWrite(RR_CHANNEL,0);
  //   ledcWrite(RL_CHANNEL,p2);
  // }
}

void left() {
  int p1 = map(outputPower, 0, 1023, 0, 255);
  int p2 = map(turnPower, 0, 1023, 0, 255);
  if (forwardStatus == 1) {
    ledcWrite(FR_CHANNEL, p1);
    ledcWrite(RR_CHANNEL, p1);
    ledcWrite(FL_CHANNEL, 0);
    ledcWrite(RL_CHANNEL, p2);
  } else {
    ledcWrite(FR_CHANNEL, p1);
    ledcWrite(RR_CHANNEL, p1);
    ledcWrite(FL_CHANNEL, p2);
    ledcWrite(RL_CHANNEL, 0);
  }

  //如果是前進狀態，停左前輪
  //如果是倒退狀態，停左後輪
  // if(forwardStatus==1){
  //   ledcWrite(FL_CHANNEL,0);
  //   ledcWrite(RL_CHANNEL,p2);
  // }else{
  //   ledcWrite(FL_CHANNEL,p1);
  //   ledcWrite(RL_CHANNEL,p1);
  // }
  // if(forwardStatus==1){
  //   ledcWrite(RR_CHANNEL,p1);
  //   ledcWrite(FR_CHANNEL,p1);
  // }else{
  //   ledcWrite(FL_CHANNEL,p2);
  //   ledcWrite(RL_CHANNEL,0);
  // }
}*/
