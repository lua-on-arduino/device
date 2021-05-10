#include <SerialFileTransfer.h>

void SerialFileTransfer::begin(
  SLIPSerial *slipSerial,
  OSCBridge *bridge,
  OSCLogger *logger
) {
  this->slipSerial = slipSerial;
  this->bridge = bridge;
  this->logger = logger;

  if (!sd.begin(SdioConfig(FIFO_SDIO))) {
    logger->error("SD card initialization failed.");
  }
}

/**
 * Read and send a file from the SD card with an accompanying message.
 */
void SerialFileTransfer::readFile(char *fileName) {
  if (!fileRead.open(fileName, O_READ)) {
    logger->error("Couldn't open file.");
    return;
  }

  OSCMessage message("/raw/file");
  message.add(fileName);
  bridge->sendMessage(message);

  slipSerial->beginPacket();
  while (fileRead.available()) slipSerial->write(fileRead.read());
  slipSerial->endPacket();
  
  fileRead.close();
}

/**
 * Write a byte into a previously opened file.
 */ 
void SerialFileTransfer::write(uint8_t b) { fileWrite.write(b); }

/**
 * Open and empty a file on the SD card for writing.
 */
void SerialFileTransfer::startWriteFile() {
  // Always override old content with new content.
  if (sd.exists(fileWriteName)) sd.remove(fileWriteName);
  fileWrite = sd.open(fileWriteName, FILE_WRITE);
}

/**
 * Close the file which has been written to and send a success message.
 */
void SerialFileTransfer::endWriteFile() {
  OSCMessage message("/success/file");
  message.add(fileWriteName);
  bridge->sendMessage(message);
  fileWrite.close();
}