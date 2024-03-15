#ifndef SPI_AVAILABLE_H
#define SPI_AVAILABLE_H

#include <Arduino.h>

//Check if data is available to be read from the SPI device's receive buffer

inline bool SPI_available() {
  return bitRead(SPSR, SPIF);
}

#endif