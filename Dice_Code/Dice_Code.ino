#include <Arduino.h>
#include <U8g2lib.h>

// SH1106 128x64 I2C
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

const int buttonPin = 18;
int lastResult = 1;

// Function to draw a 3D-style isometric cube for the boot screen
void draw3DCube(int x, int y, int size, int offset) {
  // Front Face
  u8g2.drawFrame(x, y + offset, size, size); 
  // Top Face (Isometric perspective)
  u8g2.drawLine(x, y + offset, x + 10, y - 10 + offset);
  u8g2.drawLine(x + size, y + offset, x + size + 10, y - 10 + offset);
  u8g2.drawLine(x + 10, y - 10 + offset, x + size + 10, y - 10 + offset);
  // Side Face
  u8g2.drawLine(x + size, y + size + offset, x + size + 10, y + size - 10 + offset);
  u8g2.drawLine(x + size + 10, y - 10 + offset, x + size + 10, y + size - 10 + offset);
}

void draw3DBoot() {
  for (int i = 0; i < 25; i++) {
    u8g2.clearBuffer();
    
    // Animate the cube bouncing/tumbling slightly
    int bounce = abs(sin(i * 0.5) * 10); 
    draw3DCube(45, 25, 30, -bounce);
    
    u8g2.sendBuffer();
    delay(60);
  }
  delay(500); // Hold final frame
}

// Standard 2D dice face
void drawDice(int x, int y, int num) {
  u8g2.drawRFrame(x, y, 40, 40, 5); 
  if (num == 1) { u8g2.drawDisc(x+20, y+20, 3); }
  else if (num == 2) { u8g2.drawDisc(x+10, y+10, 3); u8g2.drawDisc(x+30, y+30, 3); }
  else if (num == 3) { u8g2.drawDisc(x+10, y+10, 3); u8g2.drawDisc(x+20, y+20, 3); u8g2.drawDisc(x+30, y+30, 3); }
  else if (num == 4) { u8g2.drawDisc(x+10, y+10, 3); u8g2.drawDisc(x+30, y+10, 3); u8g2.drawDisc(x+10, y+30, 3); u8g2.drawDisc(x+30, y+30, 3); }
  else if (num == 5) { u8g2.drawDisc(x+10, y+10, 3); u8g2.drawDisc(x+30, y+10, 3); u8g2.drawDisc(x+20, y+20, 3); u8g2.drawDisc(x+10, y+30, 3); u8g2.drawDisc(x+30, y+30, 3); }
  else if (num == 6) { u8g2.drawDisc(x+10, y+10, 3); u8g2.drawDisc(x+30, y+10, 3); u8g2.drawDisc(x+10, y+20, 3); u8g2.drawDisc(x+30, y+20, 3); u8g2.drawDisc(x+10, y+30, 3); u8g2.drawDisc(x+30, y+30, 3); }
}

void rollAnimation() {
  for (int i = 0; i < 15; i++) {
    int tempRoll = random(1, 7);
    u8g2.clearBuffer();
    int offsetX = random(-3, 4);
    int offsetY = random(-3, 4);
    drawDice(44 + offsetX, 12 + offsetY, tempRoll);
    u8g2.sendBuffer();
    delay(40 + (i * 10));
  }
  lastResult = random(1, 7);
  u8g2.clearBuffer();
  drawDice(44, 12, lastResult);
  u8g2.sendBuffer();
}

void setup() {
  u8g2.begin();
  pinMode(buttonPin, INPUT_PULLUP);
  randomSeed(analogRead(0) + touchRead(4));
  
  // Show the 3D Boot Screen
  draw3DBoot();
  
  // Start with a 1
  u8g2.clearBuffer();
  drawDice(44, 12, 1);
  u8g2.sendBuffer();
}

void loop() {
  if (digitalRead(buttonPin) == LOW) {
    delay(50); // Debounce
    rollAnimation();
    while(digitalRead(buttonPin) == LOW);
  }
}