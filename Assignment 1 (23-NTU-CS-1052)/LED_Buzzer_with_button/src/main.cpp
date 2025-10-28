#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_W 128
#define OLED_H 64
Adafruit_SSD1306 screen(OLED_W, OLED_H, &Wire, -1);

#define LED_PIN 19
#define BTN_PIN 26
#define BUZZ_PIN 14

bool lightOn = false;
bool pressFlag = false;
bool holdFlag = false;
unsigned long startPress = 0;
const unsigned long holdDelay = 2000;

void showMessage(const char* text) {
  screen.clearDisplay();
  screen.setTextColor(SSD1306_WHITE);
  screen.setTextSize(1);
  screen.setCursor(0, 25);
  screen.println(text);
  screen.display();
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZ_PIN, OUTPUT);
  pinMode(BTN_PIN, INPUT_PULLUP);
  if (!screen.begin(SSD1306_SWITCHCAPVCC, 0x3C)) while (true);
  showMessage("initialize");
}

void loop() {
  bool btnState = digitalRead(BTN_PIN);

  if (btnState == LOW && !pressFlag) {
    pressFlag = true;
    startPress = millis();
    holdFlag = false;
  }

  if (btnState == LOW && pressFlag && !holdFlag) {
    if (millis() - startPress >= holdDelay) {
      showMessage("Buzzer ON");
      tone(BUZZ_PIN, 1500);
      delay(500);
      noTone(BUZZ_PIN);
      holdFlag = true;
    }
  }

  if (btnState == HIGH && pressFlag) {
    if (!holdFlag) {
      lightOn = !lightOn;
      digitalWrite(LED_PIN, lightOn);
      if (lightOn) showMessage("LED ON");
      else showMessage("LED OFF");
    }
    pressFlag = false;
    delay(250);
  }
}