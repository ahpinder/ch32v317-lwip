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

static struct udp_pcb *udp_server_pcb = NULL;

static void udp_server_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p,
                            const ip_addr_t *addr, u16_t port)
{
    // printf("received %d bytes from %s:%d\r\n", p->len, ipaddr_ntoa(addr), port);
    if (p == NULL) {
        return;
    }

    struct pbuf *tx_buf = pbuf_alloc(PBUF_TRANSPORT, sizeof(UDP_ACK_STRING) - 1, PBUF_RAM);
    if (tx_buf != NULL) {
        memcpy(tx_buf->payload, UDP_ACK_STRING, sizeof(UDP_ACK_STRING) - 1);
        udp_sendto(pcb, tx_buf, addr, port);
        pbuf_free(tx_buf);
    }

    pbuf_free(p);
}


void RS485_Init(void) {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_SET);
}

struct netif lwip_netif;

void LwIP_Init(void)
{
    ip4_addr_t ipaddr, netmask, gw;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_RNG, ENABLE);
    RNG_Cmd(ENABLE);

    lwip_init();

    IP4_ADDR(&ipaddr, 192, 168, 42, 10);
    IP4_ADDR(&netmask, 255, 255, 255, 0);
    IP4_ADDR(&gw, 192, 168, 42, 1);

    netif_add(&lwip_netif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, netif_input);
    netif_set_default(&lwip_netif);
    netif_set_up(&lwip_netif);

    udp_server_pcb = udp_new();
    if (udp_server_pcb != NULL) {
        err_t err = udp_bind(udp_server_pcb, IP_ADDR_ANY, UDP_SERVER_PORT);
        if (err == ERR_OK) {
            udp_recv(udp_server_pcb, udp_server_recv, NULL);
            printf("Initialized static UDP server on %d.%d.%d.%d:%d\r\n",
                   ip4_addr1(&ipaddr), ip4_addr2(&ipaddr), ip4_addr3(&ipaddr), ip4_addr4(&ipaddr),
                   UDP_SERVER_PORT);
        } else {
            printf("UDP bind failed: %d\r\n", err);
            udp_remove(udp_server_pcb);
            udp_server_pcb = NULL;
        }
    } else {
        printf("UDP PCB allocation failed\r\n");
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
	RS485_Init();
	USART_Printf_Init(115200);	
	printf("SystemClk:%d\r\n",SystemCoreClock);
	printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
    printf("LwIP Version: %s \r\n", LWIP_VERSION_STRING);
    Delay_Ms(100);
	printf("This is printf example\r\n");
    LwIP_Init();
    while(1) {
        if (eth_rx_pending) {
            // printf("ETH packet received\r\n");
            while (ethernetif_poll(&lwip_netif)) {
            }
            eth_rx_pending = 0;
        }
        sys_check_timeouts();
        Delay_Ms(1);
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

