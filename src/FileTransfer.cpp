#include <FileTransfer.h>

void FileTransfer::begin(
    SLIPSerial *slipSerial, Bridge *bridge, Logger *logger) {
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
bool FileTransfer::readFile(const char *fileName, uint16_t responseId) {
  if (!sd.exists(fileName)) {
    bridge->sendResponse(
        Bridge::ResponseError, responseId, "file doesn't exist");
    return false;
  }

  if (!fileRead.open(fileName, O_READ)) {
    bridge->sendResponse(
        Bridge::ResponseError, responseId, "couldn't open file");
    return false;
  }

  bridge->sendRawResponse(Bridge::ResponseSuccess, responseId);

  slipSerial->beginPacket();
  while (fileRead.available())
    slipSerial->write(fileRead.read());
  slipSerial->endPacket();

  fileRead.close();
  return true;
}

void FileTransfer::deleteFile(const char *fileName, uint16_t responseId) {
  if (sd.remove(fileName)) {
    bridge->sendResponse(Bridge::ResponseSuccess, responseId);
  } else {
    bridge->sendResponse(Bridge::ResponseError, responseId);
  }
}

void FileTransfer::renameFile(
    const char *oldName, const char *newName, uint16_t responseId) {
  if (!sd.exists(oldName)) {
    bridge->sendResponse(
        Bridge::ResponseError, responseId, "file doesn't exist");
    return;
  }

  if (sd.rename(oldName, newName)) {
    bridge->sendResponse(Bridge::ResponseSuccess, responseId);
  } else {
    bridge->sendResponse(Bridge::ResponseSuccess, responseId);
  }
}
/**
 * Write a byte into a previously opened file.
 */
void FileTransfer::write(uint8_t b) { fileWrite.write(b); }

/**
 * Open and empty a file on the SD card for writing.
 */
void FileTransfer::startWriteFile(
    const char *dirName, const char *baseName, uint16_t responseId) {
  writeResponseId = responseId;

  // Make sure the directory exists and create all missing parent directories
  // if necessary.
  sd.mkdir(dirName, true);
  sd.chdir(dirName);

  // Always override old content with new content.
  if (sd.exists(baseName)) sd.remove(baseName);
  fileWrite = sd.open(baseName, FILE_WRITE);

  // Switch back to root.
  sd.chdir();

  if (!fileWrite) {
    bridge->sendResponse(Bridge::ResponseError, writeResponseId);
  }
}

/**
 * Close the file which has been written to and send a success message.
 */
void FileTransfer::endWriteFile() {
  if (fileWrite) {
    bridge->sendResponse(Bridge::ResponseSuccess, writeResponseId);
  }
  fileWrite.close();
}

void FileTransfer::createDirectory(const char *dirName, uint16_t responseId) {
  if (sd.mkdir(dirName, true)) {
    bridge->sendResponse(Bridge::ResponseSuccess, responseId);
  } else {
    bridge->sendResponse(Bridge::ResponseError, responseId);
  }
}

void FileTransfer::deleteDirectory(const char *dirName, uint16_t responseId) {
  if (sd.rmdir(dirName)) {
    bridge->sendResponse(Bridge::ResponseSuccess, responseId);
  } else {
    bridge->sendResponse(Bridge::ResponseError, responseId);
  }
}

void FileTransfer::listDirectoryRecursive(File dir) {
  Serial.print("[");
  File file;
  int index = 0;
  while (true) {
    file = dir.openNextFile();
    if (!file) break;

    // Finish the last file entry with a comma.
    if (index > 0) Serial.print(",");

    char fileName[256];
    file.getName(fileName, 256);

    if (file.isDirectory()) {
      Serial.printf("[\"%s\",", fileName);
      listDirectoryRecursive(file);
      Serial.print("]");
    } else {
      Serial.printf("\"%s\"", fileName);
    }

    file.close();
    index++;
  }

  Serial.print("]");
}

/**
 * Print a directory (and it's subdirectories) to the Serial.
 * The format is a json array in the following format:
 * [
 *  "my-file-1.txt",
 *  "my-file-2.txt",
 *   ["my-sub-directory", [
 *     "my-file-3.txt",
 *     ...
 *   ]]
 * ]
 */
void FileTransfer::listDirectory(const char *dirName, uint16_t responseId) {
  File dir;
  dir.open(dirName);

  if (!dir) bridge->sendResponse(Bridge::ResponseError, responseId);

  bridge->sendRawResponse(Bridge::ResponseSuccess, responseId);
  slipSerial->beginPacket();
  listDirectoryRecursive(dir);
  slipSerial->endPacket();

  dir.close();
}