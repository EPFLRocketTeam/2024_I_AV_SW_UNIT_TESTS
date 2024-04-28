#include <Arduino.h>
#include <SPI.h>

struct Packet {
    union {
        uint8_t raw[5];
        struct {
            uint8_t id;
            union {
                uint32_t as_u32;
                int32_t as_i32;
                float as_f32;
            };
        };
    };
};

Packet incoming;
Packet outgoing; 

uint32_t packetRX = 0;

void setup() {
    outgoing.id = 0x02;
    outgoing.as_f32 = 3.14;
    
    Serial.begin(9600);

    Serial.println("========================");
    Serial.println("UNIT TEST 01 > SLAVE");
    Serial.println("========================");
    Serial.println("");

    // turn on SPI in slave mode
    SPCR |= _BV(SPE);

    // turn on interrupts
    SPCR |= _BV(SPIE);

    // have to send on master in, *slave out*
    pinMode(MISO, OUTPUT);

    // now turn on interrupts
    SPI.attachInterrupt();

    // initialize the slave
    SPI.begin();
    
}

ISR (SPI_STC_vect) {
    incoming.raw[packetRX++] = SPDR;
    SPDR = outgoing.raw[packetRX++];
}

void loop() {
    if(packetRX == 5){
        Serial.print("Received packet at timefame [");
        Serial.print(millis());
        Serial.println("]: ");
        Serial.print("\t Id: ");
        Serial.println(incoming.id);
        Serial.print("\t as U32: ");
        Serial.println(incoming.as_u32);
        Serial.print("\t as I32: ");
        Serial.println(incoming.as_i32);
        Serial.print("\t as F32: ");
        Serial.println(incoming.as_f32);

        packetRX = 0;
    }
}