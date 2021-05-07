#include <LuaOnArduino.h>

LuaOnArduino::LuaOnArduino(SLIPSerial *slipSerial) {
  this->slipSerial = slipSerial;
}

void LuaOnArduino::begin() {
  logger.begin(slipSerial);
  fileTransfer.begin(slipSerial, &logger);
}

/**
 * Send an SLIP packet containing an OSC message and empty the message
 * afterwards.
 * @param message The OSC message.
 */
void LuaOnArduino::sendMessage(OSCMessage &message) {
  slipSerial->beginPacket();
  message.send(*slipSerial);
  slipSerial->endPacket();
  message.empty();
}

/**
 * Dispatch incoming OSC messages.
 */
void LuaOnArduino::handleOscInput(OSCBundle &oscInput) {
  if (oscInput.hasError()) return;

  static LuaOnArduino* that = this;

  oscInput.dispatch("/read-file", [](OSCMessage &message) {
    char fileName[that->fileNameLength];
    message.getString(0, fileName, fileNameLength);
    that->readFile(fileName);
  });

  oscInput.dispatch("/file", [](OSCMessage &message) {
    message.getString(0, that->fileTransfer.fileWriteName, fileNameLength);
    that->fileTransfer.startWriteFile();
  });

  if (oscInputHandler != NULL) oscInputHandler(oscInput);
}

/**
 * Read and send a file from the SD card with an accompanying message.
 */
void LuaOnArduino::readFile(char *fileName) {
  if (!fileTransfer.startReadFile(fileName)) {
    logger.errorBegin();
    slipSerial->printf("Couldn't open file `%s`.", fileName);
    logger.logEnd();
    return;
  }

  OSCMessage message("/file");
  message.add(fileName);
  sendMessage(message);

  slipSerial->beginPacket();
  fileTransfer.readFile();
  slipSerial->endPacket();
}

/**
 * Read from the serial and, depending on which `serialReadMode` we're in, treat
 * the received data as an OSC input or a file.
 */
void LuaOnArduino::update() {
  OSCBundle oscInput;

  if (slipSerial->available()) {
    while (!slipSerial->endOfPacket()) {
      while (slipSerial->available()) {
        int c = slipSerial->read();
        if (readSerialMode == ReadSerialModeOSC) {
          oscInput.fill(c);
        } else if (readSerialMode == ReadSerialModeFile) {
          fileTransfer.write(c);
        }
      }
    }
  }

  if (readSerialMode == ReadSerialModeOSC) {
    handleOscInput(oscInput);
  } else if (readSerialMode == ReadSerialModeFile) {
    OSCMessage message("/success/file");
    message.add(fileTransfer.fileWriteName);
    sendMessage(message);

    readSerialMode = ReadSerialModeOSC;
    fileTransfer.endWriteFile();
  }
}