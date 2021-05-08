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
  static const uint16_t fileNameLength = that->fileTransfer.fileNameLength;
  static char fileName[fileNameLength];

  oscInput.dispatch("/read-file", [](OSCMessage &message) {
    message.getString(0, fileName, fileNameLength);
    that->fileTransfer.readFile(fileName);
  });

  oscInput.dispatch("/file", [](OSCMessage &message) {
    message.getString(0, that->fileTransfer.fileWriteName, fileNameLength);
    // Start reading tha raw serial input. The readSerialMode is switched back
    // to OSC mode as soon as the raw slip packet ends.
    that->bridge.setReadSerialMode(OSCBridge::ReadSerialModeRaw);
    that->fileTransfer.startWriteFile();
  });

  oscInput.dispatch("/lua/execute-file", [](OSCMessage &message) {
    message.getString(0, fileName, fileNameLength);
    
    if (that->lua.executeFile(fileName)) {
      OSCMessage message("/success/lua/execute-file");
      that->bridge.sendMessage(message);
    } else {
      that->logger.error("Couldn't execute lua file");
    }
  });
}