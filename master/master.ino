#include <SPI.h>
#include <Arduino.h>
//#include <SPI_available.h>

inline bool SPI_available();


// MASTER - BOARD 1 //

void setup() {
  SPI.begin();
  Serial.begin(9600);
  digitalWrite(SS, HIGH);
  SPI.setClockDivider(SPI_CLOCK_DIV8);
}

void loop() {
  digitalWrite(SS, LOW);
  String message = "SPEAK";
  for(char c : message) {
    SPI.transfer(c);
  }
  digitalWrite(SS, HIGH);

  delay(500);

  digitalWrite(SS, LOW);
  while(SPI.transfer(0) != '\0') {}
  while(SPI_available()) {
    char c = SPI.transfer(0);
    Serial.print(c);
  }
  digitalWrite(SS, HIGH);

  Serial.println();
  Serial.println("MESSAGE : A SIGNAL HAS BEEN SENT TO BOARD 2 (THE SLAVE)");
  delay(1000);
}

inline bool SPI_available() {
  return bitRead(SPSR, SPIF);
}
