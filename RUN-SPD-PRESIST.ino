#include <SoftwareSerial.h>
#define PIN_LED 10
#define PIN_PHOTO_SENSOR A0

SoftwareSerial SerialBt(4, 5); // RX, TX

unsigned char data[4] = {0, 0, 0, 0};

int light = 0;
int maxLight = 300;
int numTurn = 0;
float sredSpeed = 0;
int sum = -1;
int count_zamer = 11;
int segment = 750; //75cm
unsigned long lastMillis = 0;
unsigned long currentMillis = 0;
bool work = true;

unsigned long timing;


void setup()
{
    //Serial.begin(115200);
    SerialBt.begin(9600);
    pinMode(PIN_PHOTO_SENSOR, INPUT);
    pinMode(PIN_LED, OUTPUT);

    digitalWrite(PIN_LED, HIGH);

}

void loop()
{


  light = analogRead(PIN_PHOTO_SENSOR);

  //light = random(298,302);
  //Serial.println("TEST");
  //delay(500);


  if(light > maxLight && work){

    work = false;

    if(sum == -1){
      lastMillis = millis();
      sum = 0;
    } else{

      if(millis() - lastMillis > 10)
      {
        currentMillis = millis();
        sum += (currentMillis - lastMillis);
        numTurn++;

        if(numTurn == count_zamer){
          numTurn = 0;
          sredSpeed = (float) segment / ((float)sum / (float)count_zamer);
          
          //Serial.print("SPEED: ");
          //Serial.println(sredSpeed);
          
          sum = -1;
        }
        
        lastMillis = currentMillis;
             
      }
    }
  }

  if(light <= maxLight){
    work = true;
  }
  
  if (millis() - timing > 50){

    static char outstr[4];
    dtostrf(sredSpeed,4, 2, outstr);
    String outlow = String(outstr[2]) + String(outstr[3]);

    int speed2 = map(outlow.toInt(), 0, 99, 0, 255);
    
    data[0] = 4;  //Running
    data[1] = speed2;
    data[2] = (int) sredSpeed; 
    //data[3] = spm; 
    SerialBt.write(data,4);
 
    timing = millis(); 
  }
    
}
