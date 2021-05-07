#include <SerialFileTransfer.h>

void SerialFileTransfer::begin(SLIPSerial *slipSerial, OSCLogger *logger) {
  this->slipSerial = slipSerial;
  if (!sd.begin(SdioConfig(FIFO_SDIO))) {
    logger->error("SD card initialization failed.");
  }
}

/**
 * Open a file for reading
 */
bool SerialFileTransfer::startReadFile(char *fileName) {
  return fileRead.open(fileName, O_READ);
}

/**
 * Read a file and send it to the serial.
 */
void SerialFileTransfer::readFile() {
  while (fileRead.available()) slipSerial->write(fileRead.read());  
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
void SerialFileTransfer::endWriteFile() { fileWrite.close(); }