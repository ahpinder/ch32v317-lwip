#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define RS485_DATA_LEN 64

#define PACKET_MAGIC 0xBD3D

#define PACKET_VERSION 0x00

#define PACKET_TYPE_STATUS_REQUEST 0x00
#define PACKET_TYPE_STATUS_RESPONSE 0x01
#define PACKET_TYPE_TIME_BASE_REQUEST 0x02
#define PACKET_TYPE_TIME_BASE_RESPONSE 0x03
#define PACKET_TYPE_OCTAL_STREAM_REQUEST 0x04
#define PACKET_TYPE_OCTAL_STREAM_RESPONSE 0x05

// packet flags
#define PACKET_FLAG_REQUEST_DROPPED 0x0001

#pragma pack(push, 1)

typedef struct {
    uint16_t magic;
    uint8_t pkt_type;
    uint8_t version;

    uint16_t sequence; // request: host TX sequence number. response: board sequence number
    uint16_t flags;

    uint32_t t1_us; // request: host time. response: board time at request packet RX
    uint32_t t2_us; // request: expected time delay to next request, us, 0 if unchanged. response: board time at response packet TX
} PacketHeader_t;

typedef struct {
    PacketHeader_t header;
    uint8_t buffer_lengths[8];
    uint8_t buffers[8][RS485_DATA_LEN];
} OctalPacket_t;


#pragma pack(pop)

void populate_octal_stream_request_header(OctalPacket_t* pkt);
void populate_octal_stream_response_header(OctalPacket_t* pkt);

uint8_t verify_octal_stream_request_header(const OctalPacket_t* pkt);

#ifdef __cplusplus
}
#endif
