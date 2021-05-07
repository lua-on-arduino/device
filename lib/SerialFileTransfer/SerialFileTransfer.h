#ifndef SerialFileTransfer_h
#define SerialFileTransfer_h

#include <SdFat.h>
#include <SPI.h>
#include <SLIPSerial.h>
#include <OSCLogger.h>

class SerialFileTransfer {
public:
  SLIPSerial *slipSerial;
  OSCLogger *logger;

  SdFat sd;
  File fileRead;
  File fileWrite;

  static const uint16_t fileNameLength = 256;
  char fileWriteName[fileNameLength];

  void begin(SLIPSerial *slipSerial, OSCLogger *logger);

  void write(uint8_t b);
  void startWriteFile();
  void endWriteFile();

  bool startReadFile(char* fileName);
  void readFile();
};

#endif