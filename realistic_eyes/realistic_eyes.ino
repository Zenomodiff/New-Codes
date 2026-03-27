#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

// SH1106 128x64 I2C
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// Position and Physics Variables
float curX = 0, curY = 0;     // Current pupil position
float tarX = 0, tarY = 0;     // Target pupil position
float lidH = 22, tarLid = 22; // Eyelid height (22=open, 0=closed)
unsigned long nextMoveTime = 0;
unsigned long nextBlinkTime = 0;

void setup() {
  u8g2.begin();
  randomSeed(analogRead(0));
}

void drawEye(int centerX, int centerY, float pX, float pY, float lh) {
  if (lh > 1.5) {
    // 1. The Sclera (White part)
    u8g2.setDrawColor(1);
    u8g2.drawFilledEllipse(centerX, centerY, 24, (int)lh);
    
    // 2. The Pupil (Black part)
    u8g2.setDrawColor(0);
    // Add a tiny bit of "Saccade" jitter to the pupil position
    float jitterX = random(-5, 6) * 0.1; 
    float jitterY = random(-5, 6) * 0.1;
    u8g2.drawDisc(centerX + (int)pX + jitterX, centerY + (int)pY + jitterY, 9);
    
    // 3. The Glint (White reflection - stays top-right of pupil)
    u8g2.setDrawColor(1);
    u8g2.drawDisc(centerX + (int)pX + 3, centerY + (int)pY - 3, 2);
  } else {
    // 4. Closed Eye (Just a slightly curved line)
    u8g2.setDrawColor(1);
    u8g2.drawHLine(centerX - 20, centerY, 40);
  }
}

void loop() {
  u8g2.clearBuffer();

  // --- 1. SMOOTH MOVEMENT MATH ---
  // The 0.15 factor creates a "lazy" organic follow effect
  curX += (tarX - curX) * 0.15;
  curY += (tarY - curY) * 0.15;
  lidH += (tarLid - lidH) * 0.25; // Blinks are faster than gazes

  // --- 2. GAZE LOGIC ---
  if (millis() > nextMoveTime) {
    tarX = random(-12, 13); // Look left/right
    tarY = random(-6, 7);   // Look up/down
    nextMoveTime = millis() + random(500, 3000); // Stay still for a bit
  }

  // --- 3. BLINK LOGIC ---
  if (millis() > nextBlinkTime) {
    tarLid = 0; // Trigger close
    if (lidH < 1.0) { // Once nearly closed...
      tarLid = 22;    // Trigger open
      nextBlinkTime = millis() + random(2000, 6000); // Next blink in 2-6 seconds
    }
  }

  // --- 4. RENDER BOTH EYES ---
  drawEye(38, 32, curX, curY, lidH); // Left Eye
  drawEye(90, 32, curX, curY, lidH); // Right Eye

  u8g2.sendBuffer();
  delay(10); // High refresh rate for smoothness
}