#include <SPI.h>
#include <Arduino.h>

// SLAVE - BOARD 2 //

char buf [100];
volatile byte pos;
volatile boolean process_it;
const String response = "ECHO";

void setup(void) {
  Serial.begin(9600);   // debugging
  SPCR |= bit(SPE);    // turn on SPI in slave mode
  pinMode(MISO, OUTPUT);  // have to send on master in, *slave out*
  
  // get ready for an interrupt 
  pos = 0;   // buffer empty
  process_it = false;

  // now turn on interrupts
  SPI.attachInterrupt();
}

// SPI interrupt routine
ISR(SPI_STC_vect) {
  byte c = SPDR;  // grab byte from SPI Data Register
  // add to buffer if room
  if(pos < (sizeof(buf) - 1)) {
    buf [pos++] = c;
  }
  // example: newline means time to process buffer
  if(c == '\n') {
    process_it = true;
  }
}

// main loop - wait for flag set in interrupt routine
void loop(void) {
  if(process_it) {
    buf [pos] = 0;  // adds a null terminator '\0' to the end of the buffer 'buf'
    String receivedMessage(buf);
    if(receivedMessage == "SPEAK") {
      for(char c : response) {
        SPI.transfer(c);
      }
      Serial.println(response);
      Serial.println("MESSAGE : A SIGNAL HAS BEEN SENT TO THE BOARD 2 ");
      Serial.println("RESPONSE : OK");
    }
    pos = 0;
    process_it = false;
  }  // end of flag set
}
