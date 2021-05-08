#include <OSCBridge.h>

void OSCBridge::begin(SLIPSerial *slipSerial) {
  this->slipSerial = slipSerial;
}

/**
 * Send an SLIP packet containing an OSC message and empty the message
 * afterwards.
 * @param message The OSC message.
 */
void OSCBridge::sendMessage(OSCMessage &message) {
  slipSerial->beginPacket();
  message.send(*slipSerial);
  slipSerial->endPacket();
  message.empty();
}

void OSCBridge::handleOscInput(OSCBundle &oscInput) {
  if (oscInputHandler != NULL) oscInputHandler(oscInput);
}

void OSCBridge::handleRawInput(uint8_t c) {
  if (rawInputHandler != NULL) rawInputHandler(c);
}

void OSCBridge::handleRawInputEnd() {
  if (rawInputEndHandler != NULL) rawInputEndHandler();
}

void OSCBridge::onOscInput(OscInputHandler handler) {
  oscInputHandler = handler;
}

void OSCBridge::onRawInput(RawInputHandler handler) {
  rawInputHandler = handler;
}

void OSCBridge::onRawInputEnd(RawInputEndHandler handler) {
  rawInputEndHandler = handler;
}

void OSCBridge::setReadSerialMode(ReadSerialMode mode) {
  readSerialMode = mode;
}

/**
 * Read from the serial and, depending on which `serialReadMode` we're in, treat
 * the received data as an OSC input or a file.
 */
void OSCBridge::update() {
  OSCBundle oscInput;

  if (slipSerial->available()) {
    while (!slipSerial->endOfPacket()) {
      while (slipSerial->available()) {
        int c = slipSerial->read();
        if (readSerialMode == ReadSerialModeOSC) {
          oscInput.fill(c);
        } else if (readSerialMode == ReadSerialModeRaw) {
          handleRawInput(c);
        }
      }
    }
  }

  if (readSerialMode == ReadSerialModeOSC) {
    handleOscInput(oscInput);
  } else if (readSerialMode == ReadSerialModeRaw) {
    handleRawInputEnd();
    readSerialMode = ReadSerialModeOSC;
  }
}