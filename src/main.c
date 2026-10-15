/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 *@Note
 USART Print debugging routine:
 USART1_Tx(PA9).
 This example demonstrates using USART1(PA9) as a print debug port output.

*/

#include "debug.h"
#include "main.h"
#include "string.h"
#include "eth_driver.h"

#include "lwip/init.h"
#include "lwip/timeouts.h"
#include "lwip/netif.h"
#include "lwip/udp.h"
#include "lwip/ip_addr.h"

#include "ethernetif.h"


/* Global typedef */

/* Global define */
#define UDP_SERVER_PORT 3000
#define UDP_ACK_STRING "ACK"

#define UART_RX_DMA_SIZE 128
#define UART_TX_DMA_SIZE 128

#define START_TRANSMIT GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_SET)
#define STOP_TRANSMIT GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_RESET)

volatile uint8_t uart_rx_dma_buffer[UART_RX_DMA_SIZE];
volatile uint8_t uart_tx_dma_buffer[UART_TX_DMA_SIZE];

volatile RS485_Buffers_t buffers = {
    .tx_buffers = {uart_tx_dma_buffer},
    .num_tx_bytes = {0},
    .rx_buffers = {uart_rx_dma_buffer},    
    .num_rx_bytes = {0},
    .rx_buffer_len = UART_RX_DMA_SIZE
};

volatile uint8_t debug_int_flag = 0;

static struct udp_pcb *udp_server_pcb = NULL;
static ip_addr_t udp_peer_addr;

static void udp_server_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p,
                            const ip_addr_t *addr, u16_t port)
{
    // printf("received %d bytes from %s:%d\r\n", p->len, ipaddr_ntoa(addr), port);
    if (p == NULL) {
        return;
    }

    // struct pbuf *tx_buf = pbuf_alloc(PBUF_TRANSPORT, sizeof(UDP_ACK_STRING) - 1, PBUF_RAM);
    // if (tx_buf != NULL) {
    //     memcpy(tx_buf->payload, UDP_ACK_STRING, sizeof(UDP_ACK_STRING) - 1);
    //     udp_sendto(pcb, tx_buf, addr, port);
    //     pbuf_free(tx_buf);
    // }
    ip_addr_copy(udp_peer_addr, *addr);
    uint16_t tx_len = (p->len > UART_TX_DMA_SIZE) ? UART_TX_DMA_SIZE : p->len;
    memcpy(uart_tx_dma_buffer, p->payload, tx_len);
    transmit_uart1(uart_tx_dma_buffer, tx_len);
    // printf("transmitted %d bytes\r\n", tx_len);

    pbuf_free(p);
}


// void RS485_Init(void) {
// 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
// 	GPIO_InitTypeDef GPIO_InitStructure = {0};
// 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
//     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
//     GPIO_Init(GPIOA, &GPIO_InitStructure);
//     GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_SET);
// }

// void RS485_RX_Init(void) {
//     GPIO_InitTypeDef GPIO_InitStructure = {0};
//     RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
//     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
//     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
//     GPIO_Init(GPIOA, &GPIO_InitStructure);
// }

// void USART1_DMA_Init(void) {
//     /* UART1  TX-->DMA1_Channel4   RX-->DMA1_Channel5 */
//     DMA_InitTypeDef DMA_InitStructure = {0};
// 	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1|RCC_AHBPeriph_DMA2, ENABLE);
// 	DMA_DeInit(DMA1_Channel4);
// 	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART1->DATAR);
// 	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)uart_tx_dma_buffer;
// 	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
// 	//DMA_InitStructure.DMA_BufferSize = size(TX_DMA_dataBuf);
// 	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
// 	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
// 	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
// 	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
// 	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
// 	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
// 	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
// 	DMA_Init(DMA1_Channel4, &DMA_InitStructure);

// 	DMA_DeInit(DMA1_Channel5);
// 	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART1->DATAR);
// 	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)uart_rx_dma_buffer;
// 	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
// 	DMA_InitStructure.DMA_BufferSize = (uint32_t)UART_RX_DMA_SIZE;
// 	DMA_Init(DMA1_Channel5, &DMA_InitStructure);

// 	DMA_ClearFlag(DMA1_IT_TC4);

// 	DMA_Cmd(DMA1_Channel5, ENABLE);
// 	USART_DMACmd(USART1,USART_DMAReq_Rx|USART_DMAReq_Tx,ENABLE);

// }

// void UART_Interrupt_Init(void)
// {
// 	NVIC_InitTypeDef NVIC_InitStructure={0};

// 	/* UART1  TX-->DMA1_Channel4   RX-->DMA1_Channel5 */
// 	DMA_ITConfig(DMA1_Channel4,DMA_IT_TC,ENABLE);
//     USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
//     // USART_ITConfig(USART1, USART_IT_TC, ENABLE); // gets enabled in interrupt

// 	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;
// 	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
// 	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
// 	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
// 	NVIC_Init(&NVIC_InitStructure);
//     NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
//     NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//     NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
//     NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//     NVIC_Init(&NVIC_InitStructure);
// }

void transmit_uart1(uint8_t *data, uint16_t size) {
    GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_SET);
    memcpy(uart_tx_dma_buffer, data, size);
    /* update uart tx dma address */
    DMA1_Channel4->MADDR = (uint32_t)uart_tx_dma_buffer;

    /* update uart tx dma length */
    DMA1_Channel4->CNTR = size;

    /* enable uart tx dma */
    DMA_Cmd(DMA1_Channel4,ENABLE);

}

struct netif lwip_netif;

// Sends data to the most recent UDP peer. Returns ERR_CONN if no peer is known yet.
static err_t eth_transmit(const uint8_t *data, uint16_t len) {
    if (udp_server_pcb == NULL ) {
        return ERR_CONN;
    }
    struct pbuf *tx_buf = pbuf_alloc(PBUF_TRANSPORT, len, PBUF_RAM);
    if (tx_buf == NULL) {
        return ERR_MEM;
    }
    memcpy(tx_buf->payload, data, len);
    err_t err = udp_sendto(udp_server_pcb, tx_buf, &udp_peer_addr, UDP_SERVER_PORT);
    pbuf_free(tx_buf);
    return err;
}

void LwIP_Init(void)
{
    ip4_addr_t ipaddr, netmask, gw;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_RNG, ENABLE);
    RNG_Cmd(ENABLE);

    lwip_init();

    IP4_ADDR(&ipaddr, 192, 168, 42, 10);
    IP4_ADDR(&netmask, 255, 255, 255, 0);
    IP4_ADDR(&gw, 192, 168, 42, 1);

    printf("before netif_add\r\n");
    netif_add(&lwip_netif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, netif_input);
    printf("after netif_add\r\n");
    netif_set_default(&lwip_netif);
    printf("netif set up\r\n");
    netif_set_up(&lwip_netif);
    printf("netif is up\r\n");

    udp_server_pcb = udp_new();
    if (udp_server_pcb != NULL) {
        err_t err = udp_bind(udp_server_pcb, IP_ADDR_ANY, UDP_SERVER_PORT);
        if (err == ERR_OK) {
            udp_recv(udp_server_pcb, udp_server_recv, NULL);
            // printf("Initialized static UDP server on %d.%d.%d.%d:%d\r\n",
                //    ip4_addr1(&ipaddr), ip4_addr2(&ipaddr), ip4_addr3(&ipaddr), ip4_addr4(&ipaddr),
                //    UDP_SERVER_PORT);
        } else {
            // printf("UDP bind failed: %d\r\n", err);
            udp_remove(udp_server_pcb);
            udp_server_pcb = NULL;
        }
    } else {
        // printf("UDP PCB allocation failed\r\n");
    }
}


void ResetPHY() {
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    GPIO_WriteBit(GPIOE, GPIO_Pin_7, Bit_RESET);
    Delay_Ms(50);
    GPIO_WriteBit(GPIOE, GPIO_Pin_7, Bit_SET);
    Delay_Ms(1000);
}

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
	// uint8_t macaddr[] = {0xe4, 0x66, 0xe5, 0x56, 0xd3, 0xbe};
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	SystemCoreClockUpdate();
	Delay_Init();
	ResetPHY();
    RS485_Init(&buffers, 115200);
	// RS485_Init();
	// USART_Printf_Init(115200);
    // RS485_RX_Init();
    // USART1_DMA_Init();
    // UART_Interrupt_Init();
	// printf("SystemClk:%d\r\n",SystemCoreClock);
	// printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
    // printf("LwIP Version: %s \r\n", LWIP_VERSION_STRING);
    // Delay_Ms(100);
	// printf("This is printf example\r\n");
    // char inc_base = 'a';
    // char msg[] = "test_txa\r\n";

    // char msg[100] = {0};
    // while(1) {
    //     // msg[7] = inc_base + debug_int_flag;
    //     uint16_t num_rx_bytes = buffers.num_rx_bytes[0];
    //     buffers.num_rx_bytes[0] = 0;
    //     START_TRANSMIT;
    //     uint16_t len = sprintf(msg, "Received %d bytes in last window\r\n", num_rx_bytes);
    //     transmit_uart1((uint8_t *)msg, len);
    //     // msg[7] = inc_base + (msg[7] + 1 - inc_base) % 10;
    //     // Delay_Ms(10);
    //     // STOP_TRANSMIT;
    //     Delay_Ms(5000);
    // }

    uint8_t src_buffer[64];
    uint8_t dest_buffer[64];
    memset(src_buffer, 0, sizeof(src_buffer));
    memset(dest_buffer, 0, sizeof(dest_buffer));
    START_TRANSMIT;
    printf("Starting transmission test...\r\n");
    // while(1) {
    //     uint32_t before = current_time_us();
    //     for (uint16_t i = 0; i < 0x100; i++) {
    //         src_buffer[i >> 6] += dest_buffer[(i + 1) >> 6] + 1;
    //         memcpy(dest_buffer, src_buffer, sizeof(src_buffer));
    //     }
    //     uint32_t after = current_time_us();
    //     printf("Time elapsed (us) for 0x100 memcpy operations: %d. non optimization byte: %d\r\n", after - before, src_buffer[0]);
    //     Delay_Ms(1000); // Add a delay to avoid flooding the output
    // }

    LwIP_Init();
    printf("amogus\r\n");
    while(1) {
        if (eth_rx_pending) {
            // printf("ETH packet received\r\n");
            while (ethernetif_poll(&lwip_netif)) {
            }
            eth_rx_pending = 0;
        }
        if (buffers.num_rx_bytes[0] != 0) {
            uint16_t len = buffers.num_rx_bytes[0];
            if (len > UART_RX_DMA_SIZE) {
                len = UART_RX_DMA_SIZE;
            }
            eth_transmit((const uint8_t *)uart_rx_dma_buffer, len);
            buffers.num_rx_bytes[0] = 0;
        }
        sys_check_timeouts();
        Delay_Us(100);
    }

	// ETH_Init(macaddr);
	// LinkNegotiate_PurelyAuto();
	// uint16_t controlregstate = ETH_ReadPHYRegister(PHY_ADDRESS, 0x00);
    // controlregstate &= 0xEFFF;
    // ETH_WritePHYRegister(PHY_ADDRESS, 0x00, controlregstate);
	// Delay_Ms(50);
	// // controlregstate = ETH_ReadPHYRegister(PHY_ADDRESS, 0x00);
    // // controlregstate |= 0x1000;
    // // ETH_WritePHYRegister(PHY_ADDRESS, 0x00, controlregstate);
	// controlregstate = ETH_ReadPHYRegister(PHY_ADDRESS, 0x00);
    // controlregstate |= 0x0200;
    // ETH_WritePHYRegister(PHY_ADDRESS, 0x00, controlregstate);
	// uint16_t status = 0x0000;
    // while((status & 0x0004) == 0x0000) {
    //     status = ETH_ReadPHYRegister(PHY_ADDRESS, 0x01);
    //     printf("status after init: %02x\r\n", status);
    //     Delay_Ms(100);
    // }
    // // test integer divide speed
    // uint32_t num_divs_per_test = 0x1000;
    // uint64_t dividend = 0x1234567812345678;
    // uint32_t divisor = 0x99;
    // uint64_t noopt_accumulator = 0;
    // while (1) {
    //     uint32_t before = current_time_us();
    //     for (uint32_t i = 0; i < num_divs_per_test; i++) {
    //         // volatile uint64_t result = dividend / (divisor + i);
    //         // noopt_accumulator += result;
    //         current_time_us();
    //     }
    //     uint32_t after = current_time_us();
    //     printf("noopt_accumulator: %016llx\r\n", noopt_accumulator);
    //     printf("time is %d\r\n", current_time_ms());
    //     printf("time elapsed (us): %d\r\n", after - before);
    //     Delay_Ms(500);
    // }

    // uint8_t cnt = 0x00;
	// while(1)
    // {
	// 	// uint32_t int_sta;
	// 	// int_sta = ETH->DMASR;
	// 	// printf("int_sta: %08x\r\n", int_sta);
	// 	// printf("DMAIER: %08x\r\n", ETH->DMAIER);
    //     printf("hello world %02x\r\n", cnt);
    //     cnt++;
	// 	Delay_Ms(100);
	// }
}

