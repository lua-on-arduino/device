#ifndef LuaOnArduino_h
#define LuaOnArduino_h

#include <Bridge.h>
#include <FileTransfer.h>
#include <Logger.h>
#include <LuaWrapper.h>
#include <OSCBundle.h>
#include <OSCMessage.h>
#include <SLIPSerial.h>

class LuaOnArduino {
private:
  typedef void (*CustomInstaller)();
  typedef void (*ResetHandler)();
  typedef void (*OscInputHandler)(OSCMessage &oscInput);

  CustomInstaller customInstaller;
  ResetHandler handleReset;
  OscInputHandler oscInputHandler;

  void setupLuaFirmware();

public:
  SLIPSerial *slipSerial;
  Logger logger;
  Bridge bridge;
  FileTransfer fileTransfer;
  Lua lua;
  static const uint16_t maxFileNameLength = 256;

  LuaOnArduino(SLIPSerial *slipSerial);
  void handleOscInput(OSCMessage &oscInput);
  void begin();
  void reset(bool runEntry = false);
  void update();

  void onInstall(CustomInstaller installer);
  void beforeReset(ResetHandler handler);
  void onOscInput(OscInputHandler handler);
};

#endif