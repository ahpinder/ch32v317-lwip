#ifndef RS485_H
#define RS485_H

#include "ch32v30x.h"

#define NUM_RS485_CHANNELS 1

typedef struct {
    uint8_t* tx_buffers[NUM_RS485_CHANNELS]; // array of pointers to tx buffers
    uint16_t num_tx_bytes[NUM_RS485_CHANNELS]; // array of number of bytes to transmit for each TX buffer
    uint8_t* rx_buffers[NUM_RS485_CHANNELS]; // array of pointers to rx buffers
    uint16_t num_rx_bytes[NUM_RS485_CHANNELS]; // array of number of received bytes for each RX buffer
    uint16_t rx_buffer_len; // length of rx buffers, assumed all buffers have this length
} RS485_Buffers_t;

void RS485_Pin_Init(void);
void RS485_USART_Init(uint32_t baudrate);
void RS485_DMA_Init(RS485_Buffers_t* buffers);
void RS485_Interrupt_Init(void);
void RS485_Init(RS485_Buffers_t* buffers, uint32_t baudrate);

void RS485_DMA_TC_Callback(USART_TypeDef *USARTx, uint8_t USART_num);
void RS485_USART_TC_Callback(RS485_Buffers_t* buffers, USART_TypeDef *USARTx, uint8_t USART_num);
void RS485_USART_IDLE_Callback(RS485_Buffers_t* buffers, USART_TypeDef *USARTx, uint8_t USART_num);

#endif