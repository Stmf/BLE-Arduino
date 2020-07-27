#include <Arduino.h>


#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

// подключен к контакту GPIO34 (ADC1_CH6) 
const int inputPin = 34;
const int ledPIN = 23;

unsigned char data[4] = {0, 0, 0, 0};
float lastChars[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

int light = 0;
int maxLight = 1500;
int numTurn = 0;
int count_zamer = 10;
int segment = 750; //75cm
unsigned long lastMillis = 0;
unsigned long currentMillis = 0;
unsigned long RPMtime = 0;
float RPMspeed = 0;
unsigned long oldTic = 0;

byte fakePos[1] = {1};

bool _BLEClientConnected = false;

#define RSCService BLEUUID((uint16_t)0x1814)
BLECharacteristic RSCMeasurementCharacteristics(BLEUUID((uint16_t)0x2A53), BLECharacteristic::PROPERTY_NOTIFY);
BLECharacteristic sensorPositionCharacteristic(BLEUUID((uint16_t)0x2A5D), BLECharacteristic::PROPERTY_READ);

BLEDescriptor RSCDescriptor(BLEUUID((uint16_t)0x2901));
BLEDescriptor sensorPositionDescriptor(BLEUUID((uint16_t)0x2901));

class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      _BLEClientConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      _BLEClientConnected = false;
    }
};

void InitBLE() {
  BLEDevice::init("RUN-SPD");
  // CBLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Do some BLE Setup
  BLEService *pRSC = pServer->createService(RSCService);

  pRSC->addCharacteristic(&RSCMeasurementCharacteristics);
  RSCDescriptor.setValue("Send all your RCSM rubbish here");
  RSCMeasurementCharacteristics.addDescriptor(&RSCDescriptor);
  RSCMeasurementCharacteristics.addDescriptor(new BLE2902());

  pRSC->addCharacteristic(&sensorPositionCharacteristic);


  pServer->getAdvertising()->addServiceUUID(RSCService);

  pRSC->start();

  pServer->getAdvertising()->start();
}

void setup() {

  Serial.begin(115200);

  pinMode(ledPIN, OUTPUT);
  digitalWrite(ledPIN, HIGH);

  InitBLE();

}

void loop()
{

  light = analogRead(inputPin);

  //light = random(298,302);
  //Serial.println(light);
  //delay(500);
  
  if(light < maxLight) {
    
    if(numTurn < count_zamer) {

      if((millis() - lastMillis) > 50) {

        currentMillis = millis();
      
        RPMtime = millis() - lastMillis;

        lastChars[numTurn] = (float) segment / ((float) RPMtime);

        lastMillis = currentMillis;
    
        numTurn++;
      }
    
    
    } else {
      numTurn = 0;
    }
  }

  if((millis() - oldTic) > 50 ) {
    
    RPMspeed = FindMax(lastChars, 10);

    //Serial.println("TEST");

    static char outstr[4];
    dtostrf(RPMspeed,4, 2, outstr);
    String outlow = String(outstr[2]) + String(outstr[3]);

    int speed2 = map(outlow.toInt(), 0, 99, 0, 255);
    
    data[0] = 4;  //Running
    data[1] = speed2;
    data[2] = (int) RPMspeed; 
    //data[3] = spm; 
    
    RSCMeasurementCharacteristics.setValue(data,4);

    RSCMeasurementCharacteristics.notify();

    sensorPositionCharacteristic.setValue(fakePos, 1);
    
    oldTic = millis();
  }
  
}

float FindMax(float* mass,int count) 
{
    if (count < 1)
      return 0;
    float arr_max=mass[0];

    for(int i = 1; i <= count; i++){
     arr_max = max(arr_max, mass[i]);
    }
    return arr_max;
}
