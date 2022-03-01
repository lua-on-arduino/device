#ifndef SerialFileTransfer_h
#define SerialFileTransfer_h

#include <Bridge.h>
#include <Logger.h>
#include <SLIPSerial.h>
#include <SPI.h>
#include <SdFat.h>

// Todo: make this work for all arduinos, this is only a quick and dirty
// workaround for the teensy.
// See: https://github.com/greiman/SdFat/issues/291#issuecomment-903292293
typedef FsFile File;

class FileTransfer {
private:
  uint16_t writeResponseId;
  void listDirectoryRecursive(File dir);

public:
  SLIPSerial *slipSerial;
  Bridge *bridge;
  Logger *logger;

  SdFat sd;
  File fileRead;
  File fileWrite;

  void begin(SLIPSerial *slipSerial, Bridge *bridge, Logger *logger);

  void write(uint8_t b);
  bool readFile(const char *fileName, uint16_t responseId);
  void deleteFile(const char *fileName, uint16_t responseId);
  void renameFile(
      const char *oldName, const char *newName, uint16_t responseId);
  void startWriteFile(
      const char *dirName, const char *baseName, uint16_t responseId);
  void endWriteFile();

  void listDirectory(const char *dirName, uint16_t responseId);
  void createDirectory(const char *dirName, uint16_t responseId);
  void deleteDirectory(const char *dirName, uint16_t responseId);
};

#endif