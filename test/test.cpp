#include <Arduino.h>
#include <unity.h>

#include "../tests/LuaWrapper.test.cpp"
#include "../tests/FileTransfer.test.cpp"

void setUp() {
  LuaWrapperTests::setUp();
  FileTransferTests::setUp();
}

void setup() {
  while (!Serial) {}

  UNITY_BEGIN();
  LuaWrapperTests::run();
  FileTransferTests::run();
  UNITY_END();
}

void loop() {}