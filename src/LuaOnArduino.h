#ifndef LuaOnArduino_h
#define LuaOnArduino_h

#include <SLIPSerial.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <OSCLogger.h>
#include <SerialFileTransfer.h>

class LuaOnArduino {
public:
  SLIPSerial* slipSerial;
  OSCLogger logger;
  static const uint16_t fileNameLength = 256;

private:
  enum ReadSerialMode {
    ReadSerialModeOSC,
    ReadSerialModeFile
  };

  typedef void (*OscInputHandler)(OSCBundle &oscInput);

  SerialFileTransfer fileTransfer;
  ReadSerialMode readSerialMode = ReadSerialModeOSC;
  OscInputHandler oscInputHandler;

  void handleOscInput(OSCBundle &oscInput);
  void readFile(char *fileName);

public:
  LuaOnArduino(SLIPSerial *slipSerial);
  void begin();

  void sendMessage(OSCMessage &message);
  void sendRaw();

  void update();
  
  void onOscInput(OscInputHandler handler) { oscInputHandler = handler; }
};

#endif