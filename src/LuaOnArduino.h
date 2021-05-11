#ifndef LuaOnArduino_h
#define LuaOnArduino_h

#include <SLIPSerial.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <Logger.h>
#include <Bridge.h>
#include <FileTransfer.h>
#include <LuaWrapper.h>

class LuaOnArduino {
public:
  SLIPSerial* slipSerial;
  Logger logger;
  Bridge bridge;
  FileTransfer fileTransfer;
  Lua lua;
  static const uint16_t maxFileNameLength = 256;

public:
  LuaOnArduino(SLIPSerial *slipSerial);

  void handleOscInput(OSCBundle &oscInput);
  void begin();
  void update();
};

#endif