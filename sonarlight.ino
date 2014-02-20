/*
  Code to integrate Maxbotix sensor and LED strips

  Instructions:
  - At least one of: (comment the appropriate code below)
    * PW is digital pin 8
    * TX is digital pin 6
    * AN is analog pin A0
  - Change code below according to your model (LV, XL and
  HRLV supported)

  Note:
  For convenience, the getRange method will always return centimeters.
  You can use convert fuctions to convert to another unit (toInches and
  toCentimeters are available)

*/
#include "Maxbotix.h"
#include <Adafruit_NeoPixel.h>

#ifndef PSTR
 #define PSTR // Make Arduino Due happy
#endif

#define STRIPPIN1 5
#define STRIPPIN2 6
#define PIXELCOUNT 26

#define BESTRANGE 132  //Eleven Feet


#define INCHESPERPIXEL 7.1 
#define SENSOROFFSET 4.0

#define RED 0xF00000
#define REDNEAR 0xFF0000
#define GREEN 0x00FF00
#define YELLOWNEAR 0xFFFF00
#define YELLOW 0xFFF000
#define OFF 0x000000

//#define DEBUGS 0
boolean firstloop=true;
//Maxbotix rangeSensorPW(8, Maxbotix::PW, Maxbotix::LV);
//Maxbotix rangeSensorTX(6, Maxbotix::TX, Maxbotix::LV);
// Enable the Maxbotix
Maxbotix rangeSensorAD(A0, Maxbotix::AN, Maxbotix::XL, Maxbotix::BEST, 9);

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)

Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(PIXELCOUNT, STRIPPIN1,NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(PIXELCOUNT, STRIPPIN2,NEO_GRB + NEO_KHZ800);


void setup()
{
#ifdef DEBUGS
  Serial.begin(115200);
#endif
  strip1.begin();
  strip2.begin();
  strip1.setBrightness(200);
  strip2.setBrightness(200);

  strip1.show();
  strip2.show();
  // Set the delay between AD readings to 10ms
  rangeSensorAD.setADSampleDelay(10);

}


void loop()
{  
    
  // Get Range in inches
 float range=rangeSensorAD.getRange()/2.54 + SENSOROFFSET;
  float sensorValue = analogRead(A0)/2.54 + SENSOROFFSET;
  uint32_t pixels= 4;
  uint32_t color = RED;
  
  if (firstloop) {
      delay(5000); //give sensor time to calibrate
#ifdef DEBUGS
      Serial.println("Color Show");     
#endif
      for (int i=1;i<3;i++) { 
        colorWipe(strip1.Color(255, 0, 0), 50); // Red
        colorWipe(strip1.Color(0, 255, 0), 50); // Green
        colorWipe(strip1.Color(0, 0, 255), 50); // Blue
      }
      firstloop=false;
#ifdef DEBUGS
      Serial.println("End Color Show");
#endif
  }

#ifdef DEBUGS
  Serial.print("AD (BEST): ");
  Serial.print(range);
  Serial.print("inches - ");
#endif
/*  Serial.print(" - Highest Mode: ");
  Serial.print(rangeSensorAD.getSampleMode(true)/2.54);
  Serial.print(" - Lowest Mode: ");
  Serial.print(rangeSensorAD.getSampleMode(false)/2.54);
  Serial.print(" - Median: ");
  Serial.print(rangeSensorAD.getSampleMedian()/2.54);
  Serial.print(" - Best: ");
  Serial.print(rangeSensorAD.getSampleBest()/2.54);   
  Serial.print(" - Sensor Value: "); 
  Serial.print(sensorValue); 
  */
#define NEARYELLOW  60.0 // Eight Feet
#define GREENCLOSE  72 //Ten Feet
#define GREENFAR  120 // Fourteen Feet
#define FARYELLOW 132// Sixteen Feet
#define FARRED  156 // Sixteen feet
  
  
 if (range <= NEARYELLOW)
    color=REDNEAR;
  else if (range <= GREENCLOSE )
    color=YELLOWNEAR;
  else if (range <= GREENFAR)
    color=GREEN;
  else if (range <= FARYELLOW)
    color=YELLOW;
  else
    color=RED;
  switch (color) {
    case REDNEAR:
      pixels=26;
      break;
    case YELLOWNEAR:
      pixels=20;
      break;
    case GREEN:
      pixels=16;
      break;
    case YELLOW:
      pixels=10;
      break;
    case RED:
      pixels=4;
      break;
    default:
      pixels=4;
      break;
  }
  
#ifdef DEBUGS
  Serial.print(" - Color: ");
  Serial.print(color,HEX);
  Serial.println();  

#endif
  
//  colorWipe(strip1.Color(255, 0, 0), 50); // Red
//  colorWipe(strip1.Color(0, 255, 0), 50); // Green
//  colorWipe(strip1.Color(0, 0, 255), 50); // Blue
  fillLength(color, pixels);
  delay(1000);
}
// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  
  for(uint16_t i=0; i<strip1.numPixels(); i++) {
      strip1.setPixelColor(i, c);
      strip1.show();
     strip2.setPixelColor(i, c);
      strip2.show();
      delay(wait);
  }
}
// Fill the dots one after the other with a color
void fillLength(uint32_t c, uint32_t length) {
  // Make sure everything is off;
  uint16_t lastpixel = PIXELCOUNT - length;
  for(uint16_t i=0; i<PIXELCOUNT; i++) {
      strip1.setPixelColor(i, OFF);
      strip2.setPixelColor(i, OFF);
  }
  // Set the number of pixels on based on the range
  for(uint16_t i=PIXELCOUNT; i>lastpixel; i--) {
      strip1.setPixelColor(i, c);
      strip2.setPixelColor(i,c);
  }
  strip1.show();
  strip2.show();

}
