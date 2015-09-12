#include <iSdio.h>
#include <utility/Sd2CardExt.h>
#include <Adafruit_NeoPixel.h>
#include <avr/power.h>

#define VOLPIN 1
#define PIN 4
#define PIXEL_NUM 10

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_NUM, PIN, NEO_GRB + NEO_KHZ800);

const int ctrl_pin = 2;

const int chipSelectPin = 4;
Sd2CardExt card;
uint8_t buffer[512];

void printByte(uint8_t value) {
  //Serial.print(value >> 4, HEX);
  //Serial.print(value & 0xF, HEX);
}

void printBytes(uint8_t* p, uint32_t len) {
  for (int i = 0; i < len; ++i) {
    printByte(p[i]);
  }
}


void setup() {
  // Initialize UART for message print.
//  //Serial.begin(9600);
  while (!//Serial) {
    ;
  }

  // Initialize SD card.
  ////Serial.print(F("\nInitializing SD card..."));
  if (card.init(SPI_HALF_SPEED, chipSelectPin)) {
    //Serial.print(F("OK"));
  } else {
    //Serial.print(F("NG"));
    abort();
  }

  memset(buffer, 0, 0x200);

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

int currentByte = 0;
int LED_R = 0;
int LED_G = 0;
int LED_B = 0;

int isDebug = 0;

void loop() {

  float vol_value = analogRead(VOLPIN);


//tetetete----n
rainbow(1);
colorWipe(strip.Color(0, 0, 0), 1); // black
colorWipe(strip.Color(0, 0, 0), 2); // black
rainbow(6);
colorWipe(strip.Color(0, 0, 0), 40); // black

//tetetete----n
rainbow(1);
colorWipe(strip.Color(0, 0, 0), 2); // black
colorWipe(strip.Color(0, 0, 0), 2); // black
rainbow(7);
colorWipe(strip.Color(0, 0, 0), 50); // black
colorWipe(strip.Color(0, 0, 0), 20); // black

  //tetetete tetetete tetetete---n
  colorWipe(strip.Color(255, 255, 255), 26); // white
  colorWipe(strip.Color(0, 0, 255), 26); // Blue
  colorWipe(strip.Color(255, 0, 255), 25); // Red
  colorWipe(strip.Color(255, 0, 255), 10); // Red
  colorWipe(strip.Color(0, 0, 0), 1); // black
  colorWipe(strip.Color(0, 0, 0), 2); // black

  //tetetete tetetete tetetete---n
  colorWipe(strip.Color(255, 255, 255), 26); // white
  colorWipe(strip.Color(0, 0, 255), 26); // Blue
  colorWipe(strip.Color(255, 0, 255), 28); // Red
  colorWipe(strip.Color(255, 0, 255), 10); // Red

  //tetetete------n  tetetete-----n
  colorWipe(strip.Color(175, 20, 0), 50); // Red  
  colorWipe(strip.Color(0, 0, 0), 2); // black
  colorWipe(strip.Color(175, 175, 175), 45); // Blue
  colorWipe(strip.Color(0, 0, 0), 10); // black
  
    //tetete te te te!!!!
    
    rainbow(1);
    colorWipe(strip.Color(0, 0, 0), 2); // black
    colorWipe(strip.Color(0, 0, 0), 4);
    rainbow(1);
  
    rainbow(1);
    colorWipe(strip.Color(0, 0, 0), 2); // black
    colorWipe(strip.Color(0, 0, 0), 4);
    rainbow(1);
    
    rainbow(10);
    colorWipe(strip.Color(0, 0, 0), 10); // black
  
  //END here
  rainbowCycle(80);
  theaterChase(strip.Color(127, 127, 127), 80); // White



  delay(450);
}


void showNeonLed3(int val){

      ////Serial.println("led array was....");
      ////Serial.println(val);

      //strip.clear();
      switch(val){
        case 0://白
          strip.clear();
        case 111://白
//          //Serial.println("yeah 111 selected");
          colorWipe(strip.Color(255, 255, 255), 80); // white
          break;
        case 222://赤
          colorWipe(strip.Color(255, 0, 0), 80); // white
          break;
        case 333://緑
          colorWipe(strip.Color(0, 255, 0), 80); // white
          break;
        case 555://青
          colorWipe(strip.Color(0, 0, 255), 80); // white
          break;
        case 999://黒 = 消す
          colorWipe(strip.Color(0, 0, 0), 10); // black
          strip.clear();
          break;
        case 100://音楽
        
            delay(450);//TOOD for debug ちょっとずらす

//tetetete----n
rainbow(1);
colorWipe(strip.Color(0, 0, 0), 1); // black
colorWipe(strip.Color(0, 0, 0), 2); // black
rainbow(6);
colorWipe(strip.Color(0, 0, 0), 40); // black

//tetetete----n
rainbow(1);
colorWipe(strip.Color(0, 0, 0), 2); // black
colorWipe(strip.Color(0, 0, 0), 2); // black
rainbow(7);
colorWipe(strip.Color(0, 0, 0), 50); // black
colorWipe(strip.Color(0, 0, 0), 20); // black

  //tetetete tetetete tetetete---n
  colorWipe(strip.Color(255, 255, 255), 26); // white
  colorWipe(strip.Color(0, 0, 255), 26); // Blue
  colorWipe(strip.Color(255, 0, 255), 25); // Red
  colorWipe(strip.Color(255, 0, 255), 10); // Red
  colorWipe(strip.Color(0, 0, 0), 1); // black
  colorWipe(strip.Color(0, 0, 0), 2); // black

  //tetetete tetetete tetetete---n
  colorWipe(strip.Color(255, 255, 255), 26); // white
  colorWipe(strip.Color(0, 0, 255), 26); // Blue
  colorWipe(strip.Color(255, 0, 255), 28); // Red
  colorWipe(strip.Color(255, 0, 255), 10); // Red

  //tetetete------n  tetetete-----n
  colorWipe(strip.Color(175, 20, 0), 50); // Red  
  colorWipe(strip.Color(0, 0, 0), 2); // black
  colorWipe(strip.Color(175, 175, 175), 45); // Blue
  colorWipe(strip.Color(0, 0, 0), 10); // black
  
    //tetete te te te!!!!
    
    rainbow(1);
    colorWipe(strip.Color(0, 0, 0), 2); // black
    colorWipe(strip.Color(0, 0, 0), 4);
    rainbow(1);
  
    rainbow(1);
    colorWipe(strip.Color(0, 0, 0), 2); // black
    colorWipe(strip.Color(0, 0, 0), 4);
    rainbow(1);
    
    rainbow(10);
    colorWipe(strip.Color(0, 0, 0), 10); // black
  
  //END here
  rainbowCycle(80);
  theaterChase(strip.Color(127, 127, 127), 80); // White

          break;
        default:
//          //Serial.println("oh my....");
          strip.setPixelColor(0, strip.Color(0,0,0));
          break;
      }

}









//パターンを表示する
void showNeon(int argPattern){
  switch (argPattern) {
    case 1:
      for(int i=0; i <5; i++){
            strip.setPixelColor(i, strip.Color(255,255,255));
            strip.show();
      }
      break;
    case 2:
        for(int i=0; i <10; i++){
        if(i < 5){
          strip.setPixelColor(i, strip.Color(255,255,255));
        }else{
          strip.setPixelColor(i, strip.Color(255,0,0  ));
        }

        strip.show();
      }
      break;
    case 3:
      for(int i=0; i <15; i++){
        if(i < 5){
          strip.setPixelColor(i, strip.Color(255,255,255));
        }else if(i < 10){
          strip.setPixelColor(i, strip.Color(255,0,0));
        }else{
          strip.setPixelColor(i, strip.Color(0,0,255));
        }

        strip.show();
      }
      break;
    case 4:
      for(int i=0; i <20; i++){
        strip.setPixelColor(i, strip.Color(139,69,19));
        strip.show();
      }
      break;
    case 5:
      strip.clear();
      theaterChaseRainbow(50);
      break;
    case 6:
      strip.clear();
      rainbowCycle(20);
      break;
    case 7:
      strip.clear();
      rainbow(20);
      break;
    case 8:
      strip.clear();
      theaterChaseRainbow(50);
      break;
    default:
      // if nothing else matches, do the default
      // default is optional
      break;
  }
}


//NEON PIXEL LIBRARRY
// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
        }
        strip.show();

        delay(wait);

        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, 0);        //turn every third pixel off
        }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if(WheelPos < 170) {
    WheelPos -= 85;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}
