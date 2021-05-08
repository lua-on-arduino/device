#ifndef OSCBridge_h
#define OSCBridge_h

#include <SLIPSerial.h>
#include <OSCMessage.h>
#include <OSCBundle.h>

class OSCBridge {
public:
  enum ReadSerialMode {
    ReadSerialModeOSC,
    ReadSerialModeRaw
  };
  
private:
  SLIPSerial *slipSerial;

  typedef void (*OscInputHandler)(OSCBundle &oscInput);
  typedef void (*RawInputHandler)(uint8_t c);
  typedef void (*RawInputEndHandler)();

  ReadSerialMode readSerialMode = ReadSerialModeOSC;
  OscInputHandler oscInputHandler;
  RawInputHandler rawInputHandler;
  RawInputEndHandler rawInputEndHandler;

  void handleOscInput(OSCBundle &oscInput);
  void handleRawInput(uint8_t c);
  void handleRawInputEnd();
public:
  void begin(SLIPSerial *slipSerial);
  void sendMessage(OSCMessage &message);
  void sendRaw();
  void setReadSerialMode(ReadSerialMode mode);
  void update();

  void onOscInput(OscInputHandler handler);
  void onRawInput(RawInputHandler handler);
  void onRawInputEnd(RawInputEndHandler handler);
};

#endif