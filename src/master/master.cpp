#include <Arduino.h>
#include <SPI.h>

#define DATA_SIZE 63
#define PAYLOAD_SIZE DATA_SIZE + 1

#define RESERVE_REMAINING(n) uint8_t reserved[DATA_SIZE - n]{}

struct intranet_packet_t {
    uint8_t id;
    union packet_data_t{
        uint8_t raw[PAYLOAD_SIZE];

        struct void_packet_t {
            RESERVE_REMAINING(0);
            uint8_t csc = 0x00;
        } void_packet;

        struct speak_packet_t {
            char payload[6];
            RESERVE_REMAINING(6);
            uint8_t csc = 0x00;
        } speak_packet;

        struct echo_packet_t {
            char payload[5];
            RESERVE_REMAINING(5);
            uint8_t csc = 0x00;
        } echo_packet;

    } data;
};

static intranet_packet_t VOID_PACKET {.id = 0x00};
static intranet_packet_t SPEAK_PACKET {.id = 0x01};
static intranet_packet_t RESPONSE_PACKET {};

void setup() {
    Serial.begin(9600);

    Serial.println("========================");
    Serial.println("UNIT TEST 01 > MASTER");
    Serial.println("========================");
    Serial.println("");

    SPI.begin();
    VOID_PACKET.data.void_packet.csc = compute_csc(VOID_PACKET);

    memcpy(SPEAK_PACKET.data.speak_packet.payload, "SPEAK", sizeof(SPEAK_PACKET.data.speak_packet.payload));
    SPEAK_PACKET.data.speak_packet.csc = compute_csc(SPEAK_PACKET);
}

void loop() {

    RESPONSE_PACKET.id = 0x00;
    memset(RESPONSE_PACKET.data.raw, 0, sizeof(RESPONSE_PACKET.data));

    SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));   
    SPI.transfer(SPEAK_PACKET.id);
    SPI.transfer(SPEAK_PACKET.data.raw, PAYLOAD_SIZE);
    
    RESPONSE_PACKET.id = SPI.transfer(VOID_PACKET.id);
    for(uint32_t i = 0; i < sizeof(intranet_packet_t); i++)
        RESPONSE_PACKET.data.raw[i] = SPI.transfer(VOID_PACKET.data.raw[i]);

    SPI.endTransaction();

    Serial.println("RESPONSE PACKET");
    Serial.println("ID: " + String(RESPONSE_PACKET.id));
    Serial.println("DATA: " + String((char*)RESPONSE_PACKET.data.raw));
    Serial.println("CSC: " + String(RESPONSE_PACKET.data.void_packet.csc));
    Serial.println("VALID ?: " + String(RESPONSE_PACKET.data.void_packet.csc == compute_csc(RESPONSE_PACKET) ? "YES" : "NO"));

    Serial.println("");
    delay(1000);
}

uint8_t compute_csc(const intranet_packet_t& packet){

    uint8_t csc = packet.id;
    for (uint32_t i = 0; i < DATA_SIZE; i++)
        csc += packet.data.raw[i];
    
    return csc;
}