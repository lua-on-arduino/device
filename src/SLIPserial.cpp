#include "SLIPSerial.h"

static const uint8_t eot = 0300;
static const uint8_t slipEsc = 0333;
static const uint8_t slipEscEnd = 0334;
static const uint8_t slipEscEsc = 0335;

SLIPSerial::SLIPSerial(usb_serial_class &s) {
  serial = &s;
  rstate = CHAR;
}

bool SLIPSerial::endOfPacket() {
  if (rstate == SECONDEOT) {
    rstate = CHAR;
    return true;
  }

  if (rstate == FIRSTEOT) {
    if (serial->available()) {
      uint8_t c = serial->peek();
      if (c == eot) {
        serial->read();
      }
    }
    rstate = CHAR;
    return true;
  }

  return false;
}

int SLIPSerial::available() {
back:
  int count = serial->available();

  if (count == 0) {
    return 0;
  }

  if (rstate == CHAR) {
    uint8_t c = serial->peek();
    if (c == slipEsc) {
      rstate = SLIPESC;
      serial->read();
      goto back;
    } else if (c == eot) {
      rstate = FIRSTEOT;
      serial->read();
      goto back;
    }
    return 1;
  } else if (rstate == SLIPESC) {
    return 1;
  } else if (rstate == FIRSTEOT) {
    if (serial->peek() == eot) {
      rstate = SECONDEOT;
      serial->read();
      return 0;
    }
    rstate = CHAR;
  } else if (rstate == SECONDEOT) {
    rstate = CHAR;
  }

  return 0;
}

int SLIPSerial::read() {
back:
  uint8_t c = serial->read();
  if (rstate == CHAR) {
    if (c == slipEsc) {
      rstate = SLIPESC;
      goto back;
    } else if (c == eot) {
      return -1;
    }
    return c;
  } else if (rstate == SLIPESC) {
    rstate = CHAR;
    if (c == slipEscEnd) {
      return eot;
    } else if (c == slipEscEsc) {
      return slipEsc;
    } else {
      return -1;
    }
  } else {
    return -1;
  }
}

int SLIPSerial::peek() {
  uint8_t c = serial->peek();
  if (rstate == SLIPESC) {
    if (c == slipEscEnd) {
      return eot;
    } else if (c == slipEscEsc) {
      return slipEsc;
    }
  }
  return c;
}

size_t SLIPSerial::write(uint8_t b) {
  if (b == eot) {
    serial->write(slipEsc);
    return serial->write(slipEscEnd);
  } else if (b == slipEsc) {
    serial->write(slipEsc);
    return serial->write(slipEscEsc);
  } else {
    return serial->write(b);
  }
}

size_t SLIPSerial::write(const uint8_t *buffer, size_t size) {
  size_t result = 0;
  while (size--) {
    result = write(*buffer++);
  }
  return result;
}

void SLIPSerial::begin(unsigned long baudrate) { serial->begin(baudrate); }

void SLIPSerial::beginPacket() { serial->write(eot); }

void SLIPSerial::endPacket() {
  serial->write(eot);
  serial->send_now();
}

void SLIPSerial::flush() { serial->flush(); }
