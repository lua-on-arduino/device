#ifndef LuaOnArduino_h
#define LuaOnArduino_h

#include <SLIPSerial.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <OSCLogger.h>
#include <OSCBridge.h>
#include <SerialFileTransfer.h>
#include <LuaWrapper.h>

class LuaOnArduino {
public:
  SLIPSerial* slipSerial;
  OSCLogger logger;
  OSCBridge bridge;
  Lua lua;

private:
  SerialFileTransfer fileTransfer;

public:
  LuaOnArduino(SLIPSerial *slipSerial);

  void handleOscInput(OSCBundle &oscInput);
  void begin();
  void update();
};

#endif