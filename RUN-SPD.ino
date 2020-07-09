#include <SoftwareSerial.h>
#define interruptPin 2

SoftwareSerial SerialBt(4, 5); // RX, TX

unsigned char data[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

unsigned long lastInterrupt = 0;
int unsigned long RPMtime = 0;
float RPMspeed;

void RPM()
 {
    if(millis() - lastInterrupt > 10)
    {

    RPMtime = millis() - lastInterrupt;

    lastInterrupt = millis();

    }
    
 }

void setup()
{
    Serial.begin(115200);
    SerialBt.begin(9600);
    pinMode(interruptPin, INPUT);
    attachInterrupt(digitalPinToInterrupt(interruptPin), RPM, FALLING);

}

void loop()
{

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
    
  }

  delay(50);
    
}
