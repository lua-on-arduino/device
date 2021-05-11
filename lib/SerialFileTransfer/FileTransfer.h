#ifndef SerialFileTransfer_h
#define SerialFileTransfer_h

#include <SdFat.h>
#include <SPI.h>
#include <SLIPSerial.h>
#include <Bridge.h>
#include <Logger.h>

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

  void readFile(char *fileName, uint16_t responseId);
  void write(uint8_t b);
  void startWriteFile(char *fileName, uint16_t responseId);
  void endWriteFile();

  void listDirectory(const char *dirName, uint16_t responseId);
};

#endif