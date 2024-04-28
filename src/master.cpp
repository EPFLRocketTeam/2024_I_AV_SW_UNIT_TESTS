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

void setup() {
    Serial.begin(9600);

    Serial.println("========================");
    Serial.println("UNIT TEST 01 > MASTER");
    Serial.println("========================");
    Serial.println("");

    SPI.begin();
}

void loop() {

    /**
     * TODO
     *  - Send a packet to the slave
     *  - Log response
    */
    Packet outgoing;
    outgoing.id = 0x01;
    outgoing.as_f32 = 3.14;

    Packet incoming;
    
    SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
    digitalWrite(SS, LOW);
    for (size_t i = 0; i < 5; i++){
        incoming.raw[i] = SPI.transfer(outgoing.raw[i]);
    }

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
    delay(1000);
}