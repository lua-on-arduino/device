#include <LuaOnArduino.h>

LuaOnArduino::LuaOnArduino(SLIPSerial *slipSerial) {
  this->slipSerial = slipSerial;
}

void LuaOnArduino::begin() {
  logger.begin(slipSerial);
  bridge.begin(slipSerial);
  lua.begin(slipSerial);
  fileTransfer.begin(slipSerial, &bridge, &logger);

  static LuaOnArduino *that = this;

  bridge.onOscInput([](OSCBundle &oscInput) {
    that->handleOscInput(oscInput);
  });

  bridge.onRawInput([](uint8_t c) {
    that->fileTransfer.write(c);
  });

  bridge.onRawInputEnd([]() {
    that->fileTransfer.endWriteFile();
  });

  lua.onErrorBegin([]() { that->logger.errorBegin(); });
  lua.onErrorEnd([]() { that->logger.logEnd(); });
}

void LuaOnArduino::update() { bridge.update(); }


/**
 * Dispatch incoming OSC messages.
 */
void LuaOnArduino::handleOscInput(OSCBundle &oscInput) {
  if (oscInput.hasError()) return;

  static LuaOnArduino* that = this;
  static char fileName[maxFileNameLength];

  oscInput.dispatch("/read-file", [](OSCMessage &message) {
    uint16_t responseId = message.getInt(0);
    message.getString(1, fileName, maxFileNameLength);
    that->fileTransfer.readFile(fileName, responseId);
  });

  oscInput.dispatch("/write-file", [](OSCMessage &message) {
    uint16_t responseId = message.getInt(0);
    message.getString(1, fileName, maxFileNameLength);
    // Start reading tha raw serial input. The readSerialMode is switched back
    // to OSC mode as soon as the raw slip packet ends.
    that->bridge.setReadSerialMode(Bridge::ReadSerialModeRaw);
    that->fileTransfer.startWriteFile(fileName, responseId);
  });

  oscInput.dispatch("/lua/execute-file", [](OSCMessage &message) {
    message.getString(0, fileName, maxFileNameLength);
    
    if (that->lua.executeFile(fileName)) {
      OSCMessage message("/success/lua/execute-file");
      that->bridge.sendMessage(message);
    } else {
      that->logger.error("Couldn't execute lua file");
    }
  });

  oscInput.dispatch("/list-dir", [](OSCMessage &message) {
    uint16_t responseId = message.getInt(0);
    message.getString(1, fileName, maxFileNameLength);
    that->fileTransfer.listDirectory(fileName, responseId);
  });
}