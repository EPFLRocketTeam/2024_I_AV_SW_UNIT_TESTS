#include <SPI.h>
#include <Arduino.h>
//#include <SPI_available.h>

#define INTRANET_MAX_PAYLOAD_SIZE 256

inline bool SPI_available();

struct IntranetPacket {
  uint8_t id;
  uint8_t len;
  uint8_t csc;
  uint8_t payload[INTRANET_MAX_PAYLOAD_SIZE];
};

uint8_t computeCSC(const IntranetPacket& packet);

const IntranetPacket message = {0x01, 4, computeCSC(message), {"SPEAK"}};
const String message_payload = message.payload;

// MASTER - BOARD 1 //

void setup() {
  SPI.begin();
  Serial.begin(9600);
  digitalWrite(SS, HIGH);
  SPI.setClockDivider(SPI_CLOCK_DIV8);
}

void loop() {
  digitalWrite(SS, LOW);
  for(char c : message_payload) {
    SPI.transfer(c);
  }
  digitalWrite(SS, HIGH);

  Serial.println();
  Serial.println("MESSAGE : A SIGNAL HAS BEEN SENT TO BOARD 2 (THE SLAVE)");

  delay(500);

  digitalWrite(SS, LOW);
  //while(SPI.transfer(0) != '\0') {} Je sais pas si cette fonction est essentielle ou pas 
  while(SPI_available()) {
    char c = SPI.transfer(0);
    Serial.print(c);
  }
  digitalWrite(SS, HIGH);

  delay(1000);
}

// fonctions :
uint8_t computeCSC(const IntranetPacket& packet) {
  uint8_t checksum = 0;
  checksum += packet.id;
  checksum += packet.len;
  for(int i = 0; i < packet.len; ++i) {
    checksum += packet.payload[i];
  }
  return checksum;
}

inline bool SPI_available() {
  return bitRead(SPSR, SPIF);
}
