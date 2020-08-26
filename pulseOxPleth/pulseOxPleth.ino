#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "MAX30105.h"

MAX30105 particleSensor;

Adafruit_SSD1306 oled(128, 64, &Wire, -1);
byte x;
byte y;
byte z;
byte lastx;
byte lasty;
long baseValue = 0;
long lastMin=2200000;
long lastMax=0;
long rollingMin = 2200000;
long rollingMax=0;
 
void setup() {
  Serial.begin(115200);
  particleSensor.begin(Wire, I2C_SPEED_FAST);
  oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  //Setup to sense a nice looking saw tooth on the plotter
  byte ledBrightness = 0x1F; //Options: 0=Off to 255=50mA
  byte sampleAverage = 8; //Options: 1, 2, 4, 8, 16, 32
  byte ledMode = 3; //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
  int sampleRate = 100; //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
  int pulseWidth = 411; //Options: 69, 118, 215, 411
  int adcRange = 4096; //Options: 2048, 4096, 8192, 16384

  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);

    //Take an average of IR readings at power up
  const byte avgAmount = 30;
  long reading;
  for (byte x = 0 ; x < avgAmount ; x++)
  {
    reading = particleSensor.getIR();

    if (reading > lastMax){
      lastMax = reading;
    }

    if (reading < lastMin){
      lastMin = reading;
    }
  }
  
  x = 0;
  y = 0;
  lastx = 0;
  lasty = 0;
  delay(2000);
  oled.clearDisplay();
}
 
void loop() {
  if(x>127)  
  {
    oled.clearDisplay();
    x=0;
    lastx=x;
  }

  if (z > 30) {
    z = 0;
    lastMax = rollingMax;
    lastMin = rollingMin;
    rollingMin = 2200000;
    rollingMax = 0;
  }
 
  oled.setTextColor(WHITE);
  long reading = particleSensor.getIR();
  int y=40-(map(reading, lastMin, lastMax, 0, 40));
  Serial.println(reading);
  oled.drawLine(lastx,lasty,x,y,WHITE);

  if (reading > rollingMax){
    rollingMax = reading;
  }

  if (reading < rollingMin){
    rollingMin = reading;
  }
  
  lasty=y;
  lastx=x;

  oled.display();
  x++;
  z++;
}
