#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DHT.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Pins
#define DHTPIN 23
#define DHTTYPE DHT11
#define BUTTON_PIN 5

DHT dht(DHTPIN, DHTTYPE);

// WiFi credentials
const char* ssid = "TECNO CAMON 17";
const char* password = "        12";

// -------- STATIC IP CONFIG --------
IPAddress local_IP(10,219,153,154);
IPAddress gateway( 10,219,153,139);
IPAddress subnet(255,255,255,0);
IPAddress primaryDNS(8,8,8,8);   // Optional
IPAddress secondaryDNS(1,1,1,1); // Optional
//-----------------------------------

WebServer server(80);

float lastTemp = NAN;
float lastHum = NAN;

bool lastButtonState = HIGH;

// Read DHT
void readDHTValues() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (!isnan(h) && !isnan(t)) {
    lastHum = h;
    lastTemp = t;
    Serial.print("Temp: ");
    Serial.print(t);
    Serial.print(" *C, Humidity: ");
    Serial.println(h);
  } else {
    Serial.println("Failed to read from DHT!");
  }
}

// OLED display
void showOnOLED() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  if (isnan(lastTemp) || isnan(lastHum)) {
    display.setCursor(0, 0);
    display.println("DHT Error!");
  } else {
    display.setCursor(0, 0);
    display.println("DHT22 Readings");

    display.setCursor(0, 20);
    display.print("Temp: ");
    display.print(lastTemp, 1);
    display.println(" C");

    display.setCursor(0, 40);
    display.print("Hum:  ");
    display.print(lastHum, 1);
    display.println(" %");
  }

  display.display();
}

// Webpage
void handleRoot() {
  String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<meta http-equiv='refresh' content='5'>";
  html += "<title>ESP32 DHT Monitor</title></head><body>";
  html += "<h2>ESP32 DHT22 Readings</h2>";

  if (isnan(lastTemp) || isnan(lastHum)) {
    html += "<p><b>No valid data yet.</b><br>Press the button to take a reading.</p>";
  } else {
    html += "<p><b>Temperature:</b> ";
    html += String(lastTemp, 1);
    html += " &deg;C</p>";

    html += "<p><b>Humidity:</b> ";
    html += String(lastHum, 1);
    html += " %</p>";
  }

  html += "<hr><p>Press the physical button to update readings on OLED and webpage.</p>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // OLED init
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED not found!");
    while (1);
  }

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Booting...");
  display.display();

  dht.begin();

  // ----- Apply Static IP ------
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("⚠️ Static IP failed!");
  }
  // -----------------------------

  // WiFi connect
  WiFi.begin(ssid, password);

  display.setCursor(0, 16);
  display.println("WiFi Connecting...");
  display.display();

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("WiFi Connected");

  display.setCursor(0, 16);
  display.print("IP: ");
  display.println(WiFi.localIP());

  display.setCursor(0, 32);
  display.println("Press button");
  display.setCursor(0, 44);
  display.println("to read DHT");
  display.display();

  // Web server
  server.on("/", handleRoot);
  server.begin();
}

void loop() {
  server.handleClient();

  bool currentButtonState = digitalRead(BUTTON_PIN);

  if (lastButtonState == HIGH && currentButtonState == LOW) {
    delay(50);
    if (digitalRead(BUTTON_PIN) == LOW) {
      Serial.println("Button pressed: reading DHT + updating OLED");
      readDHTValues();
      showOnOLED();
    }
  }

  lastButtonState = currentButtonState;
}
