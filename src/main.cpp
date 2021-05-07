#include <Arduino.h>
#include <SLIPSerial.h>
#include <LuaOnArduino.h>

SLIPSerial slipSerial(Serial);
LuaOnArduino luaOnArduino(&slipSerial);

void setup() {
  Serial.begin(9600);
  while (!Serial) {}
}

void loop() {}