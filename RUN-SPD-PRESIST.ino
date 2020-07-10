#include <SoftwareSerial.h>
#define PIN_LED 10
#define PIN_PHOTO_SENSOR A0

SoftwareSerial SerialBt(4, 5); // RX, TX

unsigned char data[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

float RPMspeed;

int light = 0;
int maxLight = 300;
int sum = -1;
unsigned long lastMillis = 0;
unsigned long currentMillis = 0;
bool work = true;
int unsigned long RPMtime = 0;


void setup()
{
    Serial.begin(115200);
    SerialBt.begin(9600);
    pinMode(PIN_PHOTO_SENSOR, INPUT);
    pinMode(PIN_LED, OUTPUT);

    digitalWrite(PIN_LED, HIGH);

}

void loop()
{


  light = analogRead(PIN_PHOTO_SENSOR);


  if(light > maxLight && work){

    work = false;

    if(sum == -1){
      lastMillis = millis();
      sum = 0;
    }else{

      if(millis() - lastMillis > 10)
      {
        currentMillis = millis();
        RPMtime = (currentMillis - lastMillis);
      }
      
      if(RPMtime > 0) {
        
        RPMspeed = (float) 750 / (float) RPMtime;
  
        static char outstr[4];
        dtostrf(RPMspeed,4, 2, outstr);
        String outlow = String(outstr[2]) + String(outstr[3]);

        Serial.print("TIME = ");
        Serial.print(RPMtime);
        Serial.println("");
        Serial.print("SPEED = ");
        Serial.print(RPMspeed);
        Serial.println("");

        int speed2 = map(outlow.toInt(), 0, 99, 0, 255);
    
        data[0] = 4;  //Running
        data[1] = speed2;
        data[2] = (int) RPMspeed; 
        //data[3] = spm; 
        SerialBt.write(data,10);
        
        sum = -1;

      }
      
      lastMillis = currentMillis;
    }
  }

  if(light <= maxLight){
    work = true;
  }
    
}
