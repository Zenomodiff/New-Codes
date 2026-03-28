// GY-61 (ADXL335) to Python 3D Bridge
const int xPin = 32;
const int yPin = 35;
const int zPin = 34;

void setup() {
  Serial.begin(115200);
  analogReadResolution(12); // ESP32 0-4095 range
}

void loop() {
  int x = analogRead(xPin);
  int y = analogRead(yPin);
  int z = analogRead(zPin);

  // Send as: X,Y,Z
  Serial.print(x);
  Serial.print(",");
  Serial.print(y);
  Serial.print(",");
  Serial.println(z);

  delay(30); // 50Hz update rate for smooth animation
}