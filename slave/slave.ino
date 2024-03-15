#include <SPI.h>
#include <Ar/Users/rachidkahia/Documents/Arduino/TEST1/slave/slave.inoduino.h>

inline bool SPI_available();
 
// SLAVE - BOARD 2 //

void setup() {
  SPI.begin();
  Serial.begin(9600);
  digitalWrite(SS, HIGH);
  SPI.setClockDivider(SPI_CLOCK_DIV8);
}

void loop() {
  if(digitalRead(SS) == LOW) {
    while(SPI.transfer(0) != 'S') {}
    while(SPI.transfer(0) != 'P') {}
    while(SPI.transfer(0) != 'E') {}
    while(SPI.transfer(0) != 'A') {}
    while(SPI.transfer(0) != 'K') {}

    while(SPI.transfer(0) != '\0') {}
    SPI.transfer('E');
    SPI.transfer('C');
    SPI.transfer('H');
    SPI.transfer('O');
    Serial.println("RESPONSE : OK");
  }
}

inline bool SPI_available() {
  return bitRead(SPSR, SPIF);
}