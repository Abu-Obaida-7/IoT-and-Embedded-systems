#include <Arduino.h>

// --- LED Configuration ---
#define LED_PIN1 18
#define LED_PIN2 19
#define PWM_CH1  0
#define PWM_CH2  1
#define LED_FREQ 5000
#define LED_RES  8

// --- Buzzer Configuration ---
#define BUZZER_PIN 27
#define BUZZER_CH  2
#define BUZZER_FREQ 2000
#define BUZZER_RES 10

void setup() {
  // --- Setup LED channels ---
  ledcSetup(PWM_CH1, LED_FREQ, LED_RES);
  ledcAttachPin(LED_PIN1, PWM_CH1);

  ledcSetup(PWM_CH2, LED_FREQ, LED_RES);
  ledcAttachPin(LED_PIN2, PWM_CH2);

  // --- Setup Buzzer channel ---
  ledcSetup(BUZZER_CH, BUZZER_FREQ, BUZZER_RES);
  ledcAttachPin(BUZZER_PIN, BUZZER_CH);
}

void loop() {
  // --- 1. LED Fade-In Sequence ---
  for (int d = 0; d <= 255; d++) {
    ledcWrite(PWM_CH1, d);
    ledcWrite(PWM_CH2, 255 - d); // Opposite fade effect
    delay(8);
  }

  // --- 2. LED Fade-Out Sequence ---
  for (int d = 255; d >= 0; d--) {
    ledcWrite(PWM_CH1, d);
    ledcWrite(PWM_CH2, 255 - d);
    delay(8);
  }

  // --- 3. Buzzer Melody (short cheerful tone) ---
  int melody[] = {523, 659, 587, 784, 659, 880, 784, 523}; // C5–A5 pattern
  int duration[] = {200, 200, 200, 250, 200, 250, 300, 400};

  for (int i = 0; i < 8; i++) {
    ledcWriteTone(BUZZER_CH, melody[i]);
    delay(duration[i]);
    ledcWrite(BUZZER_CH, 0);
    delay(50);
  }

  ledcWrite(BUZZER_CH, 0); // Stop buzzer
  delay(500); // small pause before repeating
}