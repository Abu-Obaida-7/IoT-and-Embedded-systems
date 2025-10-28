#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define W 128
#define H 64
Adafruit_SSD1306 scr(W, H, &Wire, -1);

#define LED_Y 4
#define LED_G 0
#define LED_R 2
#define BTN_MODE 26
#define BTN_RST 27

#define CH_Y 0
#define CH_G 1
#define CH_R 2
#define FREQ 4000
#define RES 8

hw_timer_t *blinkT = nullptr;

int modeSel = 0;
int blinkStep = 0;
bool oldMode = HIGH;
bool oldRst = HIGH;
unsigned long tPrev = 0;
const int tDelay = 600;
volatile unsigned long tick = 0;

void IRAM_ATTR timerTick() {
  tick++;
}

void showScreen() {
  scr.clearDisplay();
  scr.setTextSize(2);
  scr.setTextColor(SSD1306_WHITE);
  scr.setCursor(15, 0);
  scr.println("LED PANEL");
  scr.drawLine(0, 20, 127, 20, SSD1306_WHITE);
  scr.setTextSize(1);
  scr.setCursor(10, 35);
  if (modeSel == 0) scr.print("OFF");
  else if (modeSel == 1) scr.print("Blink");
  else if (modeSel == 2) scr.print("ON");
  else if (modeSel == 3) scr.print("PWM");
  scr.display();
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_Y, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(BTN_MODE, INPUT_PULLUP);
  pinMode(BTN_RST, INPUT_PULLUP);

  if (!scr.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (true);
  }

  ledcSetup(CH_Y, FREQ, RES);
  ledcSetup(CH_G, FREQ, RES);
  ledcSetup(CH_R, FREQ, RES);
  ledcAttachPin(LED_Y, CH_Y);
  ledcAttachPin(LED_G, CH_G);
  ledcAttachPin(LED_R, CH_R);

  blinkT = timerBegin(0, 80, true);
  timerAttachInterrupt(blinkT, &timerTick, true);
  timerAlarmWrite(blinkT, 1000000, true);
  timerAlarmEnable(blinkT);

  ledcWrite(CH_Y, 0);
  ledcWrite(CH_G, 0);
  ledcWrite(CH_R, 0);
  showScreen();
}

void loop() {
  bool nowMode = digitalRead(BTN_MODE);
  bool nowRst = digitalRead(BTN_RST);

  if (millis() - tPrev > tDelay) {
    if (nowMode == LOW && oldMode == HIGH) {
      modeSel = (modeSel + 1) % 4;
      blinkStep = 0;
      showScreen();
      tPrev = millis();
    }
    if (nowRst == LOW && oldRst == HIGH) {
      modeSel = 0;
      blinkStep = 0;
      showScreen();
      tPrev = millis();
    }
  }

  oldMode = nowMode;
  oldRst = nowRst;

  if (modeSel == 0) {
    ledcWrite(CH_Y, 0);
    ledcWrite(CH_G, 0);
    ledcWrite(CH_R, 0);
  } 
  else if (modeSel == 1) {
    static unsigned long lastTick = 0;
    if (tick != lastTick) {
      lastTick = tick;
      blinkStep = (blinkStep + 1) % 3;
      if (blinkStep == 0) {
        ledcWrite(CH_Y, 255);
        ledcWrite(CH_G, 0);
        ledcWrite(CH_R, 0);
      } else if (blinkStep == 1) {
        ledcWrite(CH_Y, 0);
        ledcWrite(CH_G, 255);
        ledcWrite(CH_R, 0);
      } else {
        ledcWrite(CH_Y, 0);
        ledcWrite(CH_G, 0);
        ledcWrite(CH_R, 255);
      }
    }
  } 
  else if (modeSel == 2) {
    ledcWrite(CH_Y, 255);
    ledcWrite(CH_G, 255);
    ledcWrite(CH_R, 255);
  } 
  else if (modeSel) {
    for (int i = 0; i <= 255 && modeSel == 3; i++) {
      ledcWrite(CH_Y, i);
      ledcWrite(CH_G, i);
      ledcWrite(CH_R, i);
      delay(5);
      if (digitalRead(BTN_MODE) == LOW || digitalRead(BTN_RST) == LOW) return;
    }
    for (int i = 255; i >= 0 && modeSel == 3; i--) {
      ledcWrite(CH_Y, i);
      ledcWrite(CH_G, i);
      ledcWrite(CH_R, i);
      delay(5);
      if (digitalRead(BTN_MODE) == LOW || digitalRead(BTN_RST) == LOW) return;
    }
  }
}