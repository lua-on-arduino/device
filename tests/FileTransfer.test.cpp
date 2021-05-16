#include <Arduino.h>
#include <unity.h>
#include <FileTransfer.h>
#include <SLIPSerial.h>
#include <Bridge.h>
#include <Logger.h>

namespace FileTransferTests {
  SLIPSerial slipSerial(Serial);
  Bridge bridge;
  Logger logger;
  FileTransfer fileTransfer;

  void FileTransfer_writeFile() {
    fileTransfer.startWriteFile("pio-test", "test.lua", 0);
    fileTransfer.endWriteFile();

    TEST_ASSERT_TRUE(fileTransfer.sd.exists("pio-test/test.lua"));
    fileTransfer.sd.remove("pio-test/test.lua");
  }

  void setUp() {
    bridge.begin(&slipSerial);
    logger.begin(&slipSerial);
    fileTransfer.begin(&slipSerial, &bridge, &logger);
    fileTransfer.sd.mkdir("pio-test");  
  }

  void run() {
    RUN_TEST(FileTransfer_writeFile);
  }
}

