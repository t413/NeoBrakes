#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <Adafruit_NeoPixel.h>

#define BTTNPIN 2
#define LIGHTPIN 3
#define PWRPIN 4
#define STRIPSIZE 8
Adafruit_NeoPixel strip = Adafruit_NeoPixel(2 * STRIPSIZE, LIGHTPIN);

void setup() {
  pinMode(BTTNPIN, INPUT);
  digitalWrite(BTTNPIN, HIGH);
  
  lights_enable();
}

void lights_enable() {
  pinMode(PWRPIN, OUTPUT);
  digitalWrite(PWRPIN, HIGH);
  delay(1);
  strip.begin();
  strip.show();
}

void lights_disable() {
  for (uint8_t i = 0; i < strip.numPixels(); i++) { strip.setPixelColor(i, 0); }
  strip.show();
  //digitalWrite(PWRPIN, LOW);
}

void loop() {
  uint8_t running = 1;
  uint8_t lastb = !digitalRead(BTTNPIN);
  uint32_t last_brake = 0;
  
  while (1) {
    uint32_t current = millis();
    uint8_t b = !digitalRead(BTTNPIN);
  
    if (b && !lastb) {
      last_brake = current; //mark time
      running = 1;
      lights_enable();
    }
    lastb = b;
  
    if (!b && running && (current - last_brake) > 30000) {
      running = 0;
      lights_disable();
    }
   
    if (running) {
      if (b) {
        uint16_t spotlight_cycle = (current >> 2) & 0xFF;
        for (uint8_t i = 0; i < STRIPSIZE; i++) {
          int16_t spotlight_sin = 100*sin((float)(spotlight_cycle*2*PI)/256.0f + i*6) + 100;
          uint32_t c = dim(0xFF0000, constrain(spotlight_sin/2+50,0,255));
          strip.setPixelColor(i, c);
          strip.setPixelColor(i + STRIPSIZE, c);
        }
      } else {
        uint16_t spotlight_cycle = (current >> 3) & 0xFF;
        uint16_t rainbow_cycle = (current >> 4) & 0xFF;
        int16_t slowSine = 50*sin((float)(rainbow_cycle*2*PI)/256.0f);
        for (uint8_t i = 0; i < STRIPSIZE; i++) {
          int16_t spotlight_sin = 100*sin((float)(spotlight_cycle*2*PI)/256.0f + i*6) + 100;
          uint32_t color = Wheel((uint32_t)(100 + slowSine + spotlight_sin) & 0xFF);
          uint32_t c = dim(color, constrain(spotlight_sin/2+50,0,255));
          strip.setPixelColor(i, c);
          strip.setPixelColor(i + STRIPSIZE, c);
        }
      }
      strip.show();
    }
  }
}


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } 
  else if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } 
  else {
    WheelPos -= 170;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

uint32_t dim(uint32_t color, uint8_t bright_r, uint8_t bright_g, uint8_t bright_b) {
  uint32_t r = map((color >> 16 & 0xFF), 0, 255, 0, bright_r);
  uint16_t g = map((color >> 8  & 0xFF), 0, 255, 0, bright_g);
  uint16_t b = map((color & 0xFF), 0, 255, 0, bright_b);
  return (r << 16) + (g << 8) + b;
}

uint32_t dim(uint32_t color, uint8_t bright) {
  return dim(color, bright, bright, bright);
}

//void sleep(uint8_t allowWakeup) {
////  Serial.println("sleeping");
//  delay(200);
//  pinMode(LIGHTPIN, INPUT);
//  digitalWrite(LIGHTPIN, LOW);
//  pinMode(PWRPIN, OUTPUT);
//  digitalWrite(PWRPIN, LOW);
//  delay(1);
//  pinMode(PWRPIN, INPUT);
//  
//  if (allowWakeup) attachInterrupt(0, pin2_isr, CHANGE);
//  else detachInterrupt(0);
//  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
//  sleep_enable();
//  sleep_mode();
//  // Upon waking up, sketch continues from this point.
//  sleep_disable();
//  
//  init_system();
//}

//void pin2_isr() { }
//
//void checkBattery(uint8_t displayStatus) {
//  long v = readVcc();
////  Serial.print("batt=");
////  Serial.println(v);
//  if (v == 3332) {
////    Serial.println("3.3v usb power");
//  } else if (v < 3400) { //battery dead!
//    if (v > 3200) {
//      init_system();
////      Serial.println("shutting down");
//      strip.setPixelColor(5, 0x000099); //blue battery-dead indicator
//      strip.show();
//      delay(2000);
//    }
//    sleep(false);
//  } else if (displayStatus) {
////    Serial.println("ok");
//    strip.setPixelColor(5, (v > 4000)? 0x009900 : (v > 3800)? 0x999900 : 0x990000); strip.show();
//    delay(600);
//    strip.setPixelColor(5, 0); strip.show();
//  }
//}
//
//long readVcc() {
//  long result;
//  // Read 1.1V reference against AVcc
//  #if defined(__AVR_AT90USB1286__) || defined(__AVR_ATmega32U4__)
//  ADMUX = _BV(REFS0) | 0b011110;
//  #else
//  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
//  #endif
//  delay(2); // Wait for Vref to settle
//  ADCSRA |= _BV(ADSC); // Convert
//  while (bit_is_set(ADCSRA,ADSC));
//  result = ADCL;
//  result |= ADCH<<8;
//  result = 1126400L / result; // Back-calculate AVcc in mV 1126400L = (1023L * 1000 * 1.1)
//  return result;
//}
//

