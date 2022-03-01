#ifndef OSCBridge_h
#define OSCBridge_h

#include <Logger.h>
#include <OSCBundle.h>
#include <OSCMessage.h>
#include <SLIPSerial.h>

class Bridge {
public:
  enum ReadSerialMode { ReadSerialModeOSC, ReadSerialModeRaw };

  enum ResponseType { ResponseSuccess, ResponseError };

private:
  SLIPSerial *slipSerial;
  Logger *logger;

  typedef void (*OscInputHandler)(OSCMessage &oscInput);
  typedef void (*RawInputHandler)(uint8_t c);
  typedef void (*RawInputEndHandler)();

  ReadSerialMode readSerialMode = ReadSerialModeOSC;
  OscInputHandler oscInputHandler;
  RawInputHandler rawInputHandler;
  RawInputEndHandler rawInputEndHandler;

  void handleOscInput(OSCMessage &oscInput);
  void handleRawInput(uint8_t c);
  void handleRawInputEnd();
  const char *getResponseAddress(ResponseType type, bool raw);

public:
  void begin(SLIPSerial *slipSerial, Logger *logger);
  void update();

  void sendRaw();
  void sendMessage(OSCMessage &message);
  void sendRawResponse(ResponseType type, uint16_t responseId);
  void sendResponse(ResponseType type, uint16_t responseId);
  template <typename T>
  void sendResponse(ResponseType type, uint16_t responseId, T argument);

  void setReadSerialMode(ReadSerialMode mode);

  void onOscInput(OscInputHandler handler);
  void onRawInput(RawInputHandler handler);
  void onRawInputEnd(RawInputEndHandler handler);
};

template <typename T>
void Bridge::sendResponse(ResponseType type, uint16_t responseId, T argument) {
  OSCMessage message(getResponseAddress(type, false));
  message.add(responseId);
  message.add(argument);
  sendMessage(message);
}

#endif