#ifndef MAIN_H
#define MAIN_H
#include <stdint.h>
#include "rs485.h"

void transmit_uart1(uint8_t *data, uint16_t size);
extern volatile uint8_t debug_int_flag;
extern volatile RS485_Buffers_t buffers;

#endif