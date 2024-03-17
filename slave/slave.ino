#include <SPI.h>
 
// SLAVE - BOARD 2 //

void setup() {
  SPI.begin();
  Serial.begin(9600);
  digitalWrite(SS, HIGH);
  SPI.setClockDivider(SPI_CLOCK_DIV8);
}

void loop() {
  if(digitalRead(SS) == LOW) {
    String receivedMessage = "";
    while(true) {
      char receivedChar = SPI.transfer(0);
      if(receivedChar == '\0') {
        break
      }
      receivedMessage += receivedChar;
    }

    if(receivedMessage == "SPEAK") {
      String response = "ECHO";
      for(char c : response) {
        SPI.transfer(0);
      }
      Serial.println("ECHO");
      Serial.println("MESSAGE : A SIGNAL HAS BEEN SENT TO BOARD 2 (THE SLAVE)");
      Serial.println("RESPONSE : OK");
    }
  }
}