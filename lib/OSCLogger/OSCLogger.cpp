#include "OSCLogger.h"

void OSCLogger::begin(SLIPSerial *slipSerial) {
  this->slipSerial = slipSerial;
}

/**
 * Send an SLIP packet containing an OSC message and empty the message
 * afterwards.
 * @param message The OSC message.
 */
void OSCLogger::sendMessage(OSCMessage &message) {
  slipSerial->beginPacket();
  message.send(*slipSerial);
  slipSerial->endPacket();
  message.empty();
}

/**
 * Return the OSC address for a log type. 
 */
const char* OSCLogger::getLogAddress(LogType type, bool raw) {
  return 
    type == LogTypeInfo && raw
    ? "/raw/log/info"
    : type == LogTypeInfo
    ? "/log/info"
    : type == LogTypeWarning && raw
    ? "/raw/log/warning"
    : type == LogTypeWarning
    ? "/log/warning"
    : type == LogTypeError && raw
    ? "/raw/log/error"
    : type == LogTypeError
    ? "/log/error"
    : "/undefined";
}

/**
 * Send an OSC "/log/{type}" message.
 */
void OSCLogger::log(LogType type, const char* text) {
  OSCMessage message(getLogAddress(type, false));
  message.add(text);
  sendMessage(message);
}

/**
 * Send an OSC "/log/info" message.
 */
void OSCLogger::info(const char* text) {
  log(LogTypeInfo, text);
}

/**
 * Send an OSC "/log/warning" message.
 */
void OSCLogger::warning(const char* text) {
  log(LogTypeWarning, text);
}

/**
 * Send an OSC "/log/error" message.
 */
void OSCLogger::error(const char* text) {
  log(LogTypeError, text);
}

void OSCLogger::dump(const char* text) {
  OSCMessage message("/dump");
  message.add(text);
  sendMessage(message);
}

void OSCLogger::logBegin(LogType type) {
  OSCMessage message(getLogAddress(type, true));
  sendMessage(message);
  slipSerial->beginPacket();
}

void OSCLogger::errorBegin() { logBegin(OSCLogger::LogTypeError); }
void OSCLogger::warningBegin() { logBegin(OSCLogger::LogTypeWarning); }
void OSCLogger::infoBegin() { logBegin(OSCLogger::LogTypeInfo); }
void OSCLogger::logEnd() { flush(); }

void OSCLogger::flush() {
  slipSerial->endPacket();
}