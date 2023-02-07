#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <esp_bt_main.h>
#include <dummy.h>



#if defined(ARDUINO_ARCH_ESP32) && defined(CONFIG_ARDUHAL_ESP_LOG)
/**
   A BLE client example that is rich in capabilities.
   There is a lot new capabilities implemented.
   author unknown
   updated by chegewara
*/
#if defined(CONFIG_ARDUHAL_ESP_LOG)
#include <ssl_client.h>
#include <ArduinoJson.h>


void line();
static BLEUUID serviceUUID("4fafc201-1fb5-459e-8fcc-c5c9c331914b");
static BLEUUID    charUUID("beb5483e-36e1-4688-b7f5-ea07361b26a8");

uint64_t chipid;
int pinADC[] = {36, 39, 34, 35, 32, 33, 25, 26, 27, 14, 12, 13, 4, 0, 2, 15};
int left = 0, right = 0, up = 0, down = 0;

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
void setup()
{
  Serial.begin(115200);
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
}


void loop()
{
  
    Serial.print("down");
    Serial.print(25);//2350down
    Serial.print("]");
    down = analogRead(25);
    Serial.print(down);
    Serial.print(" right");
    Serial.print(26);//2700right
    Serial.print("]");
    right = analogRead(26);
    Serial.print(right);
    Serial.print(" up");
    Serial.print(32);//2500up
    Serial.print("]");
    up = analogRead(32);
    Serial.print(up);
    Serial.print(" left");
    Serial.print(34);//3000left
    Serial.print("]");
    left = analogRead(34);
    Serial.print(left);
    Serial.println();

    if (down < 3100) {
      Serial.print("down");
      strnew_value = "backward";
    }
    else if (right < 2700) {
      Serial.print("right");
      strnew_value = "right";
    }
    else if (up < 2000) {
      Serial.print("up");
      strnew_value = "forward";
    }
    else if (left < 1600) {
      Serial.print("left");
      strnew_value = "left";
    }
    else {
      Serial.print("not yet");
      strnew_value = "not yet";
      }
    delay(3000);

  /*
    incomingByte = 0;
    strnew_value = "not yet";
  if (Serial.available() > 0) {
    incomingByte = Serial.read();
    Serial.print("I received: ");
    Serial.println(incomingByte, DEC);
    delay(100);
    if (incomingByte == 52)//4
      strnew_value = "right";
    else if (incomingByte == 54)//6
      strnew_value = "left";
    else if (incomingByte == 56)//8
      strnew_value = "forward";
    else if (incomingByte == 50)//2
      strnew_value = "backward";
    else if (incomingByte == 49)//8
      strnew_value = "not yet";
    else if (incomingByte == 53)//5
      strnew_value = "shoot";
    }*/
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
}

#endif
#endif
