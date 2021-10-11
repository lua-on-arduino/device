#include <Bridge.h>

void Bridge::begin(SLIPSerial *slipSerial, Logger *logger) {
  this->slipSerial = slipSerial;
  this->logger = logger;
}

/**
 * Send an SLIP packet containing an OSC message and empty the message
 * afterwards.
 * @param message The OSC message.
 */
void Bridge::sendMessage(OSCMessage &message) {
  slipSerial->beginPacket();
  message.send(*slipSerial);
  slipSerial->endPacket();
  message.empty();
}

void Bridge::sendResponse(ResponseType type, uint16_t responseId) {
  OSCMessage message(getResponseAddress(type, false));
  message.add(responseId);
  sendMessage(message);
}

void Bridge::sendRawResponse(ResponseType type, uint16_t responseId) {
  OSCMessage message(getResponseAddress(type, true));
  message.add(responseId);
  sendMessage(message);
}

void Bridge::handleOscInput(OSCMessage &oscInput) {
  if (oscInputHandler != NULL) oscInputHandler(oscInput);
}

void Bridge::handleRawInput(uint8_t c) {
  if (rawInputHandler != NULL) rawInputHandler(c);
}

void Bridge::handleRawInputEnd() {
  if (rawInputEndHandler != NULL) rawInputEndHandler();
}

void Bridge::onOscInput(OscInputHandler handler) {
  oscInputHandler = handler;
}

void Bridge::onRawInput(RawInputHandler handler) {
  rawInputHandler = handler;
}

void Bridge::onRawInputEnd(RawInputEndHandler handler) {
  rawInputEndHandler = handler;
}

void Bridge::setReadSerialMode(ReadSerialMode mode) {
  readSerialMode = mode;
}

/**
 * Return the OSC address for a response type. 
 */
const char* Bridge::getResponseAddress(ResponseType type, bool raw) {
  return 
    type == ResponseSuccess && raw
    ? "/raw/response/success"
    : type == ResponseSuccess
    ? "/response/success"
    : type == ResponseError && raw
    ? "/raw/response/error"
    : type == ResponseError
    ? "/response/error"
    : "/undefined";
}

/**
 * Read from the serial and, depending on which `serialReadMode` we're in, treat
 * the received data as an OSC input or a file.
 */
void Bridge::update() {
  OSCMessage oscInput;

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

    if (readSerialMode == ReadSerialModeOSC) {
      if (oscInput.hasError()) {
        logger->error("OSC input has error.");
      } else {
        handleOscInput(oscInput);
      }
    } else if (readSerialMode == ReadSerialModeRaw) {
      handleRawInputEnd();
      setReadSerialMode(ReadSerialModeOSC);
    }
  }
}