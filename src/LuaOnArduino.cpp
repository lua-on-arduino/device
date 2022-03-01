#include <LuaOnArduino.h>

#include <LuaHmrLibrary.h>
#include <LuaLogLibrary.h>

LuaOnArduino::LuaOnArduino(SLIPSerial *slipSerial) {
  this->slipSerial = slipSerial;
}

void LuaOnArduino::begin() {
  logger.begin(slipSerial);
  bridge.begin(slipSerial, &logger);
  lua.begin(slipSerial);
  fileTransfer.begin(slipSerial, &bridge, &logger);

  static LuaOnArduino *that = this;

  bridge.onOscInput([](OSCMessage &oscInput) {
    that->handleOscInput(oscInput);
    if (that->oscInputHandler != NULL) that->oscInputHandler(oscInput);
  });

  bridge.onRawInput([](uint8_t c) { that->fileTransfer.write(c); });

  bridge.onRawInputEnd([]() { that->fileTransfer.endWriteFile(); });

  lua.onErrorBegin([]() { that->logger.errorBegin(); });
  lua.onErrorEnd([]() { that->logger.logEnd(); });

  setupLuaFirmware();
  if (customInstaller != NULL) customInstaller();
}

void LuaOnArduino::reset(bool runEntry) {
  if (handleReset != NULL) handleReset();
  lua.reset();
  setupLuaFirmware();
  if (customInstaller != NULL) customInstaller();
  if (runEntry) lua.runFile("lua/init.lua");
}

void LuaOnArduino::setupLuaFirmware() {
  LuaLogLibrary::install(&lua, &logger);
  LuaHmrLibrary::install(&lua);
  lua.runFile("lua/loa_firmware/init.lua");
}

void LuaOnArduino::onInstall(CustomInstaller installer) {
  customInstaller = installer;
}

void LuaOnArduino::beforeReset(ResetHandler handler) { handleReset = handler; }

void LuaOnArduino::onOscInput(OscInputHandler handler) {
  oscInputHandler = handler;
}

void LuaOnArduino::update() { bridge.update(); }

/**
 * Dispatch incoming OSC messages.
 */
void LuaOnArduino::handleOscInput(OSCMessage &oscInput) {
  static LuaOnArduino *that = this;
  static char dirName[maxFileNameLength];
  static char fileName[maxFileNameLength];

  oscInput.dispatch("/read-file", [](OSCMessage &message) {
    uint16_t responseId = message.getInt(0);

    if (!message.isString(1)) {
      that->bridge.sendResponse(
          Bridge::ResponseError, responseId, "Wrong argument");
      return;
    }

    message.getString(1, fileName, maxFileNameLength);
    that->fileTransfer.readFile(fileName, responseId);
  });

  oscInput.dispatch("/write-file", [](OSCMessage &message) {
    uint16_t responseId = message.getInt(0);

    if (!message.isString(1) || !message.isString(2)) {
      that->bridge.sendResponse(
          Bridge::ResponseError, responseId, "Wrong arguments");
      return;
    }

    message.getString(1, dirName, maxFileNameLength);
    message.getString(2, fileName, maxFileNameLength);

    // Start reading the raw serial input. The readSerialMode is switched back
    // to OSC mode as soon as the raw slip packet ends.
    that->bridge.setReadSerialMode(Bridge::ReadSerialModeRaw);
    that->fileTransfer.startWriteFile(dirName, fileName, responseId);
  });

  oscInput.dispatch("/delete-file", [](OSCMessage &message) {
    uint16_t responseId = message.getInt(0);

    if (!message.isString(1)) {
      that->bridge.sendResponse(
          Bridge::ResponseError, responseId, "Wrong argument");
      return;
    }

    message.getString(1, fileName, maxFileNameLength);
    that->fileTransfer.deleteFile(fileName, responseId);
  });

  oscInput.dispatch("/create-dir", [](OSCMessage &message) {
    uint16_t responseId = message.getInt(0);

    if (!message.isString(1)) {
      that->bridge.sendResponse(
          Bridge::ResponseError, responseId, "Wrong argument");
      return;
    }

    message.getString(1, dirName, maxFileNameLength);
    that->fileTransfer.createDirectory(dirName, responseId);
  });

  oscInput.dispatch("/delete-dir", [](OSCMessage &message) {
    uint16_t responseId = message.getInt(0);

    if (!message.isString(1)) {
      that->bridge.sendResponse(
          Bridge::ResponseError, responseId, "Wrong argument");
      return;
    }

    message.getString(1, dirName, maxFileNameLength);
    that->fileTransfer.deleteDirectory(dirName, responseId);
  });

  oscInput.dispatch("/list-dir", [](OSCMessage &message) {
    uint16_t responseId = message.getInt(0);

    if (!message.isString(1)) {
      that->bridge.sendResponse(
          Bridge::ResponseError, responseId, "Wrong argument");
      return;
    }

    message.getString(1, dirName, maxFileNameLength);
    that->fileTransfer.listDirectory(dirName, responseId);
  });

  oscInput.dispatch("/lua/run-file", [](OSCMessage &message) {
    uint16_t responseId = message.getInt(0);

    if (!message.isString(1)) {
      that->bridge.sendResponse(
          Bridge::ResponseError, responseId, "Wrong argument");
      return;
    }

    message.getString(1, fileName, maxFileNameLength);

    if (that->lua.runFile(fileName)) {
      that->bridge.sendResponse(Bridge::ResponseSuccess, responseId);
    } else {
      that->bridge.sendResponse(Bridge::ResponseError, responseId);
    }
  });

  oscInput.dispatch("/lua/update-file", [](OSCMessage &message) {
    uint16_t responseId = message.getInt(0);

    if (!message.isString(1)) {
      that->bridge.sendResponse(
          Bridge::ResponseError, responseId, "Wrong argument");
      return;
    }

    message.getString(1, fileName, maxFileNameLength);

    bool usedHmr = LuaHmrLibrary::updateFile(fileName);
    if (!usedHmr) that->reset(true);

    that->bridge.sendResponse(
        Bridge::ResponseSuccess, responseId, (int)usedHmr);
  });
}