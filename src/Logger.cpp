#include "Logger.h"

void Logger::begin(SLIPSerial *slipSerial) { this->slipSerial = slipSerial; }

/**
 * Send an SLIP packet containing an OSC message and empty the message
 * afterwards.
 * @param message The OSC message.
 */
void Logger::sendMessage(OSCMessage &message) {
  slipSerial->beginPacket();
  message.send(*slipSerial);
  slipSerial->endPacket();
  message.empty();
}

/**
 * Return the OSC address for a log type.
 */
const char *Logger::getLogAddress(LogType type, bool raw) {
  return type == LogTypeInfo && raw    ? "/raw/log/info"
         : type == LogTypeInfo         ? "/log/info"
         : type == LogTypeWarn && raw  ? "/raw/log/warn"
         : type == LogTypeWarn         ? "/log/warn"
         : type == LogTypeError && raw ? "/raw/log/error"
         : type == LogTypeError        ? "/log/error"
         : type == LogTypeDump && raw  ? "/raw/log/dump"
         : type == LogTypeDump         ? "/log/dump"
                                       : "/undefined";
}

/**
 * Send an OSC "/log/{type}" message.
 */
void Logger::log(LogType type, const char *text) {
  OSCMessage message(getLogAddress(type, false));
  message.add(text);
  sendMessage(message);
}

/**
 * Send an OSC "/log/info" message.
 */
void Logger::info(const char *text) { log(LogTypeInfo, text); }

/**
 * Send an OSC "/log/warn" message.
 */
void Logger::warn(const char *text) { log(LogTypeWarn, text); }

/**
 * Send an OSC "/log/error" message.
 */
void Logger::error(const char *text) { log(LogTypeError, text); }

/**
 * Send an OSC "/log/dump" message.
 */
void Logger::dump(const char *text) { log(LogTypeDump, text); }

void Logger::logBegin(LogType type) {
  OSCMessage message(getLogAddress(type, true));
  sendMessage(message);
  slipSerial->beginPacket();
}

void Logger::errorBegin() { logBegin(Logger::LogTypeError); }
void Logger::warnBegin() { logBegin(Logger::LogTypeWarn); }
void Logger::infoBegin() { logBegin(Logger::LogTypeInfo); }
void Logger::logEnd() { flush(); }

void Logger::flush() { slipSerial->endPacket(); }