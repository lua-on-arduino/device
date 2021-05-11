#include <Arduino.h>
#include <SLIPSerial.h>
#include <LuaOnArduino.h>

SLIPSerial slipSerial(Serial);
LuaOnArduino loa(&slipSerial);

void setup() {
  while (!Serial) {}
  loa.begin();
}

void loop() {
  loa.update();
}