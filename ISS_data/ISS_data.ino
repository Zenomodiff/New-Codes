#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <ArduinoJson.h>
#include <math.h>

// OLED (U8g2)
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// WiFi
const char* ssid = "ESP32_MONITOR";
const char* password = "";

// ISS data
String latitude = "0.0";
String longitude = "0.0";

// animation
int satX = 0;
unsigned long lastFetch = 0;

// sine settings
float amplitude = 6.0;
float speed = 0.00;

// satellite bitmap (8x8)
const unsigned char satelliteBitmap[] = {
  0b11000011,
  0b11011011,
  0b00111100,
  0b01111110,
  0b00111100,
  0b11011011,
  0b11000011,
  0b00000000
};

void setup() {
  Serial.begin(115200);

  // I2C
  Wire.begin(21, 22);

  // OLED start
  u8g2.begin();

  // WiFi connect
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

// fetch ISS data
void fetchISS() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("http://api.open-notify.org/iss-now.json");

    int httpCode = http.GET();

    if (httpCode == 200) {
      String payload = http.getString();

      StaticJsonDocument<512> doc;
      deserializeJson(doc, payload);

      latitude = doc["iss_position"]["latitude"].as<String>();
      longitude = doc["iss_position"]["longitude"].as<String>();
    }

    http.end();
  }
}

// draw satellite with safe sine wave
void drawSatellite(int x) {
  float t = millis() * speed;

  int centerY = 15; // safe vertical area
  int y = centerY + (sin(t) * amplitude);

  // clamp to screen
  if (y < 0) y = 0;
  if (y > 56) y = 56;

  u8g2.drawXBMP(x, y, 8, 8, satelliteBitmap);
}

void loop() {

  // fetch every 10 seconds
  if (millis() - lastFetch > 10000) {
    fetchISS();
    lastFetch = millis();
  }

  u8g2.clearBuffer();

  // animate satellite
  drawSatellite(satX);
  satX += 2;
  if (satX > 128) satX = -10;

  // bigger font
  u8g2.setFont(u8g2_font_6x12_tr);

  // center LAT
  String latText = "LAT : " + latitude;
  int latX = (128 - u8g2.getStrWidth(latText.c_str())) / 2;

  // center LON
  String lonText = "LON : " + longitude;
  int lonX = (128 - u8g2.getStrWidth(lonText.c_str())) / 2;

  // draw text
  u8g2.drawStr(latX, 40, latText.c_str());
  u8g2.drawStr(lonX, 58, lonText.c_str());

  u8g2.sendBuffer();

  delay(50);
}