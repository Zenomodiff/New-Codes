#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

const int buttonPin = 18;
int animMode = 0; 
float timer = 0;

const int starCount = 70;
float starX[starCount], starY[starCount], starZ[starCount];

void setup() {
  u8g2.begin();
  pinMode(buttonPin, INPUT_PULLUP);
  randomSeed(analogRead(0));

  for (int i = 0; i < starCount; i++) {
    starX[i] = random(-64, 64);
    starY[i] = random(-32, 32);
    starZ[i] = random(1, 128);
  }
}

void drawSpiral() {
  for (int i = 0; i < 120; i++) {
    float angle = i * 0.2 + timer;
    float r = i * 0.4;
    u8g2.drawPixel(64 + r * cos(angle), 32 + r * sin(angle) * 0.6);
    if (i % 10 == 0) u8g2.drawDisc(64 + r * cos(angle), 32 + r * sin(angle) * 0.6, 1);
  }
}

void drawWarp() {
  for (int i = 0; i < starCount; i++) {
    starZ[i] -= 3;
    if (starZ[i] <= 0) starZ[i] = 128;
    int x = 64 + (starX[i] * 128 / starZ[i]);
    int y = 32 + (starY[i] * 128 / starZ[i]);
    if (x < 0 || x > 128 || y < 0 || y > 64) starZ[i] = 128;
    else u8g2.drawPixel(x, y);
  }
}

void drawBlackHole() {
  u8g2.drawDisc(64, 32, 5);
  for (int i = 0; i < 8; i++) {
    float r = 10 + i * 5 + (sin(timer + i) * 3);
    u8g2.drawEllipse(64, 32, r, r * 0.4);
  }
}

void drawPulsar() {
  float pulse = abs(sin(timer)) * 30;
  u8g2.drawCircle(64, 32, pulse);
  for(int i=0; i<8; i++) {
    float ang = i * 0.785 + timer;
    u8g2.drawLine(64, 32, 64 + cos(ang)*60, 32 + sin(ang)*60);
  }
}

void drawBinary() {
  int x1 = 64 + cos(timer) * 25, y1 = 32 + sin(timer) * 12;
  int x2 = 64 + cos(timer + 3.14) * 25, y2 = 32 + sin(timer + 3.14) * 12;
  u8g2.drawDisc(x1, y1, 4);
  u8g2.drawDisc(x2, y2, 2);
}

void drawNebula() {
  for (int i = 0; i < 50; i++) {
    u8g2.drawPixel(64 + sin(timer + i) * (20 + i), 32 + cos(timer * 0.5 + i) * (10 + i/2));
  }
}

void drawCluster() {
  for (int i = 0; i < 40; i++) {
    float x = 30 * cos(i * 0.5) * sin(timer + i * 0.1);
    float y = 30 * sin(i * 0.5);
    u8g2.drawPixel(64 + x, 32 + y);
    if (i % 5 == 0) u8g2.drawDisc(64 + x, 32 + y, 1);
  }
}

void drawWormhole() {
  for (int i = 0; i < 10; i++) {
    float r = fmod((timer * 25.0 + i * 15.0), 120.0); 
    if (r > 1) u8g2.drawEllipse(64, 32, (int)r, (int)(r * 0.6));
  }
}

void draw3DGalaxy() {
  for (int i = 0; i < 100; i++) {
    float angle = i * 0.15 + timer;
    float r = i * 0.5;
    float x3d = r * cos(angle);
    float z3d = r * sin(angle);
    float y3d = z3d * sin(timer * 0.5); 
    int x2d = 64 + x3d;
    int y2d = 32 + y3d;
    u8g2.drawPixel(x2d, y2d);
    if (i < 15) u8g2.drawDisc(64, 32, 2);
  }
}

void drawAsteroidBelt() {
  for (int i = 0; i < 40; i++) {
    float angle = i * 0.15 + timer * 0.5;
    float r = 35 + sin(i * 0.5) * 5; 
    float x3d = r * cos(angle);
    float z3d = r * sin(angle);
    float y3d = z3d * 0.4; 
    int px = 64 + x3d;
    int py = 32 + y3d;
    if (z3d > 0) u8g2.drawDisc(px, py, 1);
    else u8g2.drawPixel(px, py);
  }
}

void loop() {
  u8g2.clearBuffer();
  timer += 0.04;

  if (digitalRead(buttonPin) == LOW) {
    delay(50);
    if (digitalRead(buttonPin) == LOW) {
      animMode = (animMode + 1) % 10; 
      while(digitalRead(buttonPin) == LOW);
    }
  }

  switch (animMode) {
    case 0: drawSpiral(); break;
    case 1: drawWarp(); break;
    case 2: drawBlackHole(); break;
    case 3: drawPulsar(); break;
    case 4: drawBinary(); break;
    case 5: drawNebula(); break;
    case 6: drawCluster(); break;
    case 7: drawWormhole(); break;
    case 8: draw3DGalaxy(); break;
    case 9: drawAsteroidBelt(); break;
  }

  u8g2.sendBuffer();
  delay(10);
}