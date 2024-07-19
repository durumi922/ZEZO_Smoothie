#include <MsTimer2.h>                
#include <Adafruit_NeoPixel.h>       
#include <SoftwareSerial.h>          

#define RGB_PIN 7                    
#define NUMPIXELS 8                  
#define TOUCH_PIN 13                 
#define POTEN_PIN A0                 
#define Mode_EA 10                    
#define BT_RXD 8                     
#define BT_TXD 9                     
#define random_ms 200                
#define TRIG_PIN 2
#define ECHO_PIN 3

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, RGB_PIN, NEO_GRB + NEO_KHZ800);
SoftwareSerial bluetooth(BT_RXD, BT_TXD);  

int rainbow[6][3] = {
  {255, 0, 0},    
  {255, 50, 0},   
  {255, 150, 0},  
  {0, 255, 0},    
  {0, 0, 255},    
  {100, 0, 200}   
};                

int r = 0, g = 0, b = 0, Mode = 0, touch_count = 0, rainbow_count = 0, rain = 0; 
boolean touch = true, Mode_9 = true, up = true;                                  
int brightnessLevel = 255;  

unsigned long previousMillis = 0;
const long interval = 200;  

String RGB = "";
boolean RGB_Completed = false;

char currentMode = 'x';  // 현재 모드를 저장할 변수

void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  Serial.begin(9600); 
  bluetooth.begin(9600); 
  pixels.begin();   
  pixels.show();    
  pixels.setBrightness(brightnessLevel);
}

void loop() {
  if (bluetooth.available()) {
    char data = bluetooth.read(); 
    currentMode = data; 
  }

  switch (currentMode) {
    case 'r': setAllPixels(pixels.Color(255, 0, 0)); break;
    case 'o': setAllPixels(pixels.Color(255, 50, 0)); break;
    case 'y': setAllPixels(pixels.Color(255, 255, 0)); break;
    case 'g': setAllPixels(pixels.Color(0, 255, 0)); break;
    case 'b': setAllPixels(pixels.Color(0, 0, 255)); break;
    case 'p': setAllPixels(pixels.Color(100, 0, 255)); break;
    case 'w': setAllPixels(pixels.Color(255, 255, 255)); break;
    case 'i': setAllPixels(pixels.Color(255, 100, 150)); break;
    case 'x': setAllPixels(pixels.Color(0, 0, 0)); break;
    case 'a': Flutting(0, 255, 0, 500); break;
    case 'n': rainbowEffect(80); break;
    case 'c': fadeColor(pixels.Color(255, 50, 0), 50); break;
  }

  Mode_set();  
  int poten = analogRead(POTEN_PIN);  

  if (Mode != 9) {
    for (int i = 0; i < NUMPIXELS; i++) {
      pixels.setPixelColor(i, pixels.Color(map(poten, 0, 1023, 0, r), map(poten, 0, 1023, 0, g), map(poten, 0, 1023, 0, b)));
      pixels.show();
    }
    rain = 0;
  }

  if (digitalRead(TOUCH_PIN) == 1) {
    touch_count++;
    delay(10);

    if (touch_count >= 100) {
      if (r == 0 && g == 0 && b == 0) {
        b = 255;
      } else {
        r = 0;
        g = 0;
        b = 0;
        Mode = 0;
      }
      touch_count = 0;

    } else if (touch && (r != 0 || g != 0 || b != 0)) {
      Mode++;

      if (Mode > Mode_EA) {
        Mode = 1;
      }

      touch = false;
    }
  } else {
    touch = true;
    touch_count = 0;
  }

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    long duration, distance;
    
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    
    duration = pulseIn(ECHO_PIN, HIGH);
    distance = ((float)(340 * duration) / 1000) / 2;

    if (distance < 400) {
      pixels.setBrightness(brightnessLevel);
    } else {
      pixels.setBrightness(brightnessLevel / 6);
    }
  }
}

void Mode_set() {
  switch (Mode) {
    case 1: r = 255; g = 255; b = 255; break; // 중성 흰색
    case 2: r = 255; g = 100; b = 50; break; // 체리블라썸레드   
    case 3: r = 204; g = 104; b = 7; break; // 주황색
    case 4: r = 255; g = 227; b = 0; break; // 노랑
    case 5: r = 50; g = 238; b = 50; break; // 애플민트(민트그린) 
    case 6: r = 173; g = 216; b = 230; break; // 밝은 블루
    case 7: r = 255; g = 98; b = 134; break; // 핑크
    case 8: r = 148; g = 0; b = 211; break; // 보라
    case 9:
      if (Mode_9)
        MsTimer2::set(random_ms, random_set);
        MsTimer2::start();
        Mode_9 = false;
      break;
    case 10:
      if (!Mode_9) {
        MsTimer2::stop();
        Mode_9 = true;
      }
      for (int i = 0; i < NUMPIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(map(rainbow_count, 0, 1023, 0, rainbow[rain][0]), map(rainbow_count, 0, 1023, 0, rainbow[rain][1]), map(rainbow_count, 0, 1023, 0, rainbow[rain][2])));
        pixels.show();
      }

      if (up) {
        rainbow_count++;
        delayMicroseconds(500);
        if (rainbow_count == 1023)
          up = false;
      } else {
        rainbow_count--;
        delayMicroseconds(1000);
        if (rainbow_count == 0) {
          up = true;
          rain++;
          if (rain == 6)
            rain = 0;
        }
      }
      break;
  }
}

void random_set() {
  r = random(255);
  g = random(255);
  b = random(255);
}

void setAllPixels(uint32_t c) {
  for (int i = 0; i < pixels.numPixels(); i++) {
    pixels.setPixelColor(i, c);
  }
  pixels.show();
}

void Flutting(int r, int g, int b, int wait) {
  for (int i = 1; i < 6; i++) {
    setAllPixels(pixels.Color(r, g, b));
    delay(100);
    setAllPixels(pixels.Color(0, 0, 0));
    delay(wait);
  }
}

void rainbowEffect(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256; j++) {
    for (i = 0; i < pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel((i + j) & 255));
    }
    pixels.show();
    delay(wait);
  }
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
    WheelPos -= 170;
    return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}

void fadeColor(uint32_t color, int wait) {
  for (int j = 0; j < 256; j++) {
    for (int i = 0; i < pixels.numPixels(); i++) {
      pixels.setPixelColor(i, fade(pixels.getPixelColor(i), color, j / 255.0));
    }
    pixels.show();
    delay(wait);
  }
  for (int j = 255; j >= 0; j--) {
    for (int i = 0; i < pixels.numPixels(); i++) {
      pixels.setPixelColor(i, fade(pixels.getPixelColor(i), color, j / 255.0));
    }
    pixels.show();
    delay(wait);
  }
}

uint32_t fade(uint32_t fromColor, uint32_t toColor, float progress) {
  uint8_t fromRed = (fromColor >> 16) & 0xFF;
  uint8_t fromGreen = (fromColor >> 8) & 0xFF;
  uint8_t fromBlue = fromColor & 0xFF;

  uint8_t toRed = (toColor >> 16) & 0xFF;
  uint8_t toGreen = (toColor >> 8) & 0xFF;
  uint8_t toBlue = toColor & 0xFF;

  uint8_t red = fromRed + (toRed - fromRed) * progress;
  uint8_t green = fromGreen + (toGreen - fromGreen) * progress;
  uint8_t blue = fromBlue + (toBlue - fromBlue) * progress;

  return pixels.Color(red, green, blue);
}
