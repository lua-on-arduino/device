#ifndef Logger_h
#define Logger_h

#include <Arduino.h>
#include <SLIPSerial.h>
#include <OSCMessage.h>
#include <OSCBundle.h>

class Logger {
public:
  SLIPSerial *slipSerial;

  enum LogType {
    LogTypeInfo,
    LogTypeWarning,
    LogTypeError,
    LogTypeDump
  };

  void begin(SLIPSerial *slipSerial);

  void log(LogType type, const char* text);
  void info(const char* text);
  void warning(const char* text);
  void error(const char* text);
  void dump(const char* text);

  void logBegin(LogType type);
  void logEnd();
  void errorBegin();
  void warningBegin();
  void infoBegin();

  void flush();

private:
  void sendMessage(OSCMessage &message);
  const char* getLogAddress(LogType type, bool raw);
};

#endif