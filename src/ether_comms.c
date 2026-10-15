#include "ether_comms.h"

void populate_octal_stream_request_header(OctalPacket_t* pkt) {
    pkt->header.magic = PACKET_MAGIC;
    pkt->header.pkt_type = PACKET_TYPE_OCTAL_STREAM_REQUEST;
    pkt->header.version = PACKET_VERSION;
}

void populate_octal_stream_response_header(OctalPacket_t* pkt) {
    pkt->header.magic = PACKET_MAGIC;
    pkt->header.pkt_type = PACKET_TYPE_OCTAL_STREAM_RESPONSE;
    pkt->header.version = PACKET_VERSION;
}

/**
 * @brief Verify that the given OctalPacket_t has a valid request header.
 *
 * @param pkt Pointer to the OctalPacket_t to verify.
 * @return uint8_t Returns 1 if the header is valid, 0 otherwise.
 */
uint8_t verify_octal_stream_request_header(const OctalPacket_t* pkt) {
    return (pkt->header.magic == PACKET_MAGIC) &&
           (pkt->header.pkt_type == PACKET_TYPE_OCTAL_STREAM_REQUEST) &&
           (pkt->header.version == PACKET_VERSION);
}
