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
static intranet_packet_t ECHO_PACKET {.id = 0x02};
static intranet_packet_t REQUEST_PACKET {};

static volatile enum {IDLE, RECEIVED, WRITING} state;
static volatile uint32_t packetIdx = 0;

void setup() {
    Serial.begin(9600);

    Serial.println("UNIT TEST 01 > SLAVE");
    Serial.println("");

    // turn on SPI in slave mode
    SPCR |= _BV(SPE);

    // turn on interrupts
    SPCR |= _BV(SPIE);

    // have to send on master in, *slave out*
    pinMode(MISO, OUTPUT);

    // now turn on interrupts
    SPI.attachInterrupt();

    VOID_PACKET.data.void_packet.csc = compute_csc(VOID_PACKET);
    memcpy(ECHO_PACKET.data.echo_packet.payload, "ECHO", sizeof(ECHO_PACKET.data.echo_packet.payload));
    ECHO_PACKET.data.echo_packet.csc = compute_csc(ECHO_PACKET);
}

ISR (SPI_STC_vect) {
    uint8_t c = SPDR;
    SPDR = 0x00;

    switch (state)  {
    case IDLE:
        if(packetIdx++ == 0)
            REQUEST_PACKET.id = c;
        else
           REQUEST_PACKET.data.raw[packetIdx++ - 1] = c;
        

        if(REQUEST_PACKET.data.speak_packet.csc == compute_csc(REQUEST_PACKET) && packetIdx == sizeof(intranet_packet_t))
            state = RECEIVED;

        return;
    
    case WRITING:
        if(c != 0x00) //should ony receive a void packet
            return;
        if(packetIdx++ == 0)
            SPDR = ECHO_PACKET.id;
        else
            SPDR = ECHO_PACKET.data.raw[packetIdx++ - 1];

        if(packetIdx == sizeof(intranet_packet_t))
            state = IDLE;
    default:
        return;
    }
}

void loop() {
    switch (state) {
    case RECEIVED:
        Serial.println("REQUEST PACKET");
        Serial.println("ID: " + String(REQUEST_PACKET.id));
        Serial.println("DATA: " + String((char*)REQUEST_PACKET.data.raw));
        Serial.println("CSC: " + String(REQUEST_PACKET.data.void_packet.csc));
        Serial.println("VALID ?: " + String(REQUEST_PACKET.data.void_packet.csc == compute_csc(REQUEST_PACKET) ? "YES" : "NO"));
        state = WRITING;
        packetIdx = 0;
        return;
    default:
        return;
    }
}

uint8_t compute_csc(const intranet_packet_t& packet){

    uint8_t csc = packet.id;
    for (uint32_t i = 0; i < DATA_SIZE; i++)
        csc += packet.data.raw[i];
    
    return csc;
}