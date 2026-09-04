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
#include "lwip/dhcp.h"
#include "lwip/apps/httpd.h"


/* Global typedef */

/* Global define */

#define PHY_ADDRESS                             1

#define GPIO_Output(a,b) \
  GPIO_InitStructure.GPIO_Pin = b;\
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;\
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;\
  GPIO_Init(a, &GPIO_InitStructure)

#define GPIO_Input(a,b) \
  GPIO_InitStructure.GPIO_Pin = b;\
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;\
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;\
  GPIO_Init(a, &GPIO_InitStructure)

/* Global Variable */

// __attribute__((__aligned__(4))) ETH_DMADESCTypeDef DMARxDscrTab[ETH_RXBUFNB];       /* MAC receive descriptor, 4-byte aligned*/
// __attribute__((__aligned__(4))) ETH_DMADESCTypeDef DMATxDscrTab[ETH_TXBUFNB];       /* MAC send descriptor, 4-byte aligned */

// __attribute__((__aligned__(4))) uint8_t  MACRxBuf[ETH_RXBUFNB*ETH_RX_BUF_SZE];      /* MAC receive buffer, 4-byte aligned */
// __attribute__((__aligned__(4))) uint8_t  MACTxBuf[ETH_TXBUFNB*ETH_TX_BUF_SZE];      /* MAC send buffer, 4-byte aligned */

// __attribute__((__aligned__(4))) uint8_t bufcpysrc[256];
// __attribute__((__aligned__(4))) uint8_t bufcpydest[256];



void RS485_Init(void) {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_SET);
}


// void ResetPHY() {
// 	GPIO_InitTypeDef GPIO_InitStructure = {0};
// 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
//     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
//     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
//     GPIO_Init(GPIOE, &GPIO_InitStructure);
//     GPIO_WriteBit(GPIOE, GPIO_Pin_7, Bit_RESET);
//     Delay_Ms(50);
//     GPIO_WriteBit(GPIOE, GPIO_Pin_7, Bit_SET);
//     Delay_Ms(1000);
// }

// /*********************************************************************
//  * @fn      ETH_RMIIPinInit
//  *
//  * @brief   PHY RMII interface GPIO initialization.
//  *
//  * @param   none.
//  *
//  * @return  none.
//  */
// void ETH_RMIIPinInit(void)
// {
//     GPIO_InitTypeDef GPIO_InitStructure;

//     RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE|RCC_APB2Periph_AFIO, ENABLE);
//     GPIO_ETH_MediaInterfaceConfig(GPIO_ETH_MediaInterface_RMII);
//     // GPIO_Output(GPIOE, GPIO_Pin_7);                             /* RSTB */ // I added this, not sure why it wasn't there before
//     GPIO_Output(GPIOD, GPIO_Pin_8);                             /* MDIO */
//     GPIO_Output(GPIOE, GPIO_Pin_15);                            /* MDC */

//     GPIO_Output(GPIOE, GPIO_Pin_8);                             /* TXEN */
//     GPIO_Output(GPIOE, GPIO_Pin_11);                            /* TXD0 */
//     GPIO_Output(GPIOE, GPIO_Pin_10);                            /* TXD1 */

//     GPIO_Output(GPIOE, GPIO_Pin_12);                            /* REFCLK */
//     GPIO_Input(GPIOE, GPIO_Pin_9);                              /* CRSDV */
//     GPIO_Input(GPIOE, GPIO_Pin_14);                             /* RXD0 */
//     GPIO_Input(GPIOE, GPIO_Pin_13);                             /* RXD1 */
// }

// /*********************************************************************
//  * @fn      ETH_SetClock
//  *
//  * @brief   Set the Ethernet related clock
//  *
//  * @param   none.
//  *
//  * @return  none.
//  */
// void ETH_SetClock(void)
// {
//     /* Only support HSE_VALUE: 8MHz.
//      * If you select other HSE value,you should
//      * change the configuration below*/
//     RCC_PLL3Cmd(DISABLE);
//     RCC_PREDIV2Config(RCC_PREDIV2_Div1); // HSE = 8M
//     RCC_PLL3Config(RCC_PLL3Mul_12_5); // 8M*12.5 = 100MHz
//     RCC_PLL3Cmd(ENABLE);
//     while(RESET == RCC_GetFlagStatus(RCC_FLAG_PLL3RDY));
//     Delay_Us(300);//Wait for the PHY clock to stabilize
// }

// /*********************************************************************
//  * @fn      ETH_RegInit
//  *
//  * @brief   ETH register initialization.
//  *
//  * @param   ETH_InitStruct:initialization struct.
//  *          PHYAddress:PHY address.
//  *
//  * @return  Initialization status.
//  */
// uint32_t ETH_RegInit( ETH_InitTypeDef* ETH_InitStruct, uint16_t PHYAddress )
// {
//     /* Set the SMI interface clock, set as the main frequency divided by 42  */
//     ETH->MACMIIAR = (uint32_t)ETH_MACMIIAR_CR_Div42;

//     /*------------------------ MAC register configuration  -------------------------------------------*/
//     ETH->MACCR = (uint32_t)(ETH_InitStruct->ETH_Watchdog |
//                   ETH_InitStruct->ETH_Jabber |
//                   ETH_InitStruct->ETH_InterFrameGap |
//                   ETH_InitStruct->ETH_ChecksumOffload |
//                   ETH_InitStruct->ETH_AutomaticPadCRCStrip |
//                   ETH_InitStruct->ETH_LoopbackMode
//                   |(1 << 9));

//     ETH->MACFFR = (uint32_t)(ETH_InitStruct->ETH_ReceiveAll |
//                           ETH_InitStruct->ETH_SourceAddrFilter |
//                           ETH_InitStruct->ETH_PassControlFrames |
//                           ETH_InitStruct->ETH_BroadcastFramesReception |
//                           ETH_InitStruct->ETH_DestinationAddrFilter |
//                           ETH_InitStruct->ETH_PromiscuousMode |
//                           ETH_InitStruct->ETH_MulticastFramesFilter |
//                           ETH_InitStruct->ETH_UnicastFramesFilter);

//     ETH->MACHTHR = (uint32_t)ETH_InitStruct->ETH_HashTableHigh;
//     ETH->MACHTLR = (uint32_t)ETH_InitStruct->ETH_HashTableLow;

//     ETH->MACFCR = (uint32_t)((ETH_InitStruct->ETH_PauseTime << 16) |
//                      ETH_InitStruct->ETH_UnicastPauseFrameDetect |
//                      ETH_InitStruct->ETH_ReceiveFlowControl |
//                      ETH_InitStruct->ETH_TransmitFlowControl);

//     ETH->MACVLANTR = (uint32_t)(ETH_InitStruct->ETH_VLANTagComparison |
//                                ETH_InitStruct->ETH_VLANTagIdentifier);

//     ETH->DMAOMR = (uint32_t)(ETH_InitStruct->ETH_DropTCPIPChecksumErrorFrame |
//                     ETH_InitStruct->ETH_TransmitStoreForward |
//                     ETH_InitStruct->ETH_ForwardErrorFrames |
//                     ETH_InitStruct->ETH_ForwardUndersizedGoodFrames);

//     /* Reset the physical layer */
//     ETH_WritePHYRegister(PHYAddress, PHY_BCR, PHY_Reset);
//     Delay_Ms(1);
//     return ETH_SUCCESS;
// }

// /*********************************************************************
//  * @fn      PHY_FuncInit
//  *
//  * @brief   PHY register initialization.
//  *
//  * @param   None
//  *
//  * @return  None.
//  * I don't like this, what's it doing???
//  */
// void PHY_FuncInit(void)
// {
//     uint16_t regval;

//     ETH_WritePHYRegister(PHY_ADDRESS, 0x1F, 0x00 );
//     /* Configure Repeater mode */
//     regval = ETH_ReadPHYRegister(PHY_ADDRESS, 28);
//     regval |= 1<<13;
//     ETH_WritePHYRegister(PHY_ADDRESS, 28, regval );

//     /*rmii rx&tx clock change */
//     ETH_WritePHYRegister(PHY_ADDRESS, 0x1F, 0x07 );
//     regval = ETH_ReadPHYRegister( PHY_ADDRESS, 16);
//     regval &= ~(0xff<<4);
//     regval |= 0x23<<4;
//     ETH_WritePHYRegister(PHY_ADDRESS, 16, regval );
// }

// void ETH_Configuration(uint8_t *macAddr) {
//     ETH_InitTypeDef ETH_InitStructure;
//     uint16_t timeout = 10000;
	

//     /* Enable Ethernet MAC clock */
//     RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ETH_MAC | \
//                           RCC_AHBPeriph_ETH_MAC_Tx | \
//                           RCC_AHBPeriph_ETH_MAC_Rx, ENABLE);

//     /* Enable RMII GPIO */
//     ETH_RMIIPinInit();

//     /* Set the Ethernet related clock */
//     ETH_SetClock();

//     /* Reset ETHERNET on AHB Bus */
//     ETH_DeInit();

//     /* Software reset */
//     ETH_SoftwareReset();

//     /* Wait for software reset */
//     do{
//         Delay_Us(10);
//         if( !--timeout )  break;
//     }while(ETH->DMABMR & ETH_DMABMR_SR);

//     /* ETHERNET Configuration */
//     /*------------------------   MAC   -----------------------------------*/
//     ETH_InitStructure.ETH_Watchdog = ETH_Watchdog_Enable;
//     ETH_InitStructure.ETH_Jabber = ETH_Jabber_Enable;
//     ETH_InitStructure.ETH_InterFrameGap = ETH_InterFrameGap_96Bit;
// #if HARDWARE_CHECKSUM_CONFIG
//     ETH_InitStructure.ETH_ChecksumOffload = ETH_ChecksumOffload_Enable;
// #else
//     ETH_InitStructure.ETH_ChecksumOffload = ETH_ChecksumOffload_Disable;
// #endif
//     ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable;
//     ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;

//     /* Filter function configuration */
//     ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Enable;
//     ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;
//     ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Enable;
//     ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect;
//     ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;
//     ETH_InitStructure.ETH_PassControlFrames = ETH_PassControlFrames_BlockAll;
//     ETH_InitStructure.ETH_DestinationAddrFilter = ETH_DestinationAddrFilter_Normal;
//     ETH_InitStructure.ETH_SourceAddrFilter = ETH_SourceAddrFilter_Disable;

//     ETH_InitStructure.ETH_HashTableHigh = 0x0;
//     ETH_InitStructure.ETH_HashTableLow = 0x0;

//     /* VLan function configuration */
//     ETH_InitStructure.ETH_VLANTagComparison = ETH_VLANTagComparison_16Bit;
//     ETH_InitStructure.ETH_VLANTagIdentifier = 0x0;

//     /* Flow Control function configuration */
//     ETH_InitStructure.ETH_PauseTime = 0x0;
//     ETH_InitStructure.ETH_UnicastPauseFrameDetect = ETH_UnicastPauseFrameDetect_Disable;
//     ETH_InitStructure.ETH_ReceiveFlowControl = ETH_ReceiveFlowControl_Disable;
//     ETH_InitStructure.ETH_TransmitFlowControl = ETH_TransmitFlowControl_Disable;

//     /*------------------------   DMA   -----------------------------------*/
//     /* When we use the Checksum offload feature, we need to enable the Store and Forward mode:
//     the store and forward guarantee that a whole frame is stored in the FIFO, so the MAC can insert/verify the checksum,
//     if the checksum is OK the DMA can handle the frame otherwise the frame is dropped */
//     ETH_InitStructure.ETH_DropTCPIPChecksumErrorFrame = ETH_DropTCPIPChecksumErrorFrame_Enable;
//     ETH_InitStructure.ETH_TransmitStoreForward = ETH_TransmitStoreForward_Enable;
//     ETH_InitStructure.ETH_ForwardErrorFrames = ETH_ForwardErrorFrames_Enable;
//     ETH_InitStructure.ETH_ForwardUndersizedGoodFrames = ETH_ForwardUndersizedGoodFrames_Enable;
//     /* Configure Ethernet */
//     ETH_RegInit( &ETH_InitStructure, PHY_ADDRESS );

//     /* Configure MAC address */
//     ETH->MACA0HR = (uint32_t)((macAddr[5]<<8) | macAddr[4]);
//     ETH->MACA0LR = (uint32_t)(macAddr[0] | (macAddr[1]<<8) | (macAddr[2]<<16) | (macAddr[3]<<24));

//     /* Mask the interrupt that Tx good frame count counter reaches half the maximum value */
//     ETH->MMCTIMR = ETH_MMCTIMR_TGFM;
//     /* Mask the interrupt that Rx good unicast frames counter reaches half the maximum value */
//     /* Mask the interrupt that Rx crc error counter reaches half the maximum value */
//     ETH->MMCRIMR = ETH_MMCRIMR_RGUFM | ETH_MMCRIMR_RFCEM;

//     ETH_DMAITConfig(ETH_DMA_IT_NIS |\
//                 ETH_DMA_IT_R |\
//                 ETH_DMA_IT_T |\
//                 ETH_DMA_IT_AIS |\
//                 ETH_DMA_IT_RBU,\
//                 ENABLE);

//     PHY_FuncInit();
//     /*Reads the default value of the PHY_BSR register*/
//     // LastPhyStat = ETH_ReadPHYRegister( PHY_ADDRESS, PHY_BSR );
// }

// /*********************************************************************
//  * @fn      ETH_Init
//  *
//  * @brief   Ethernet initialization.
//  *
//  * @return  none
//  */
// void ETH_Init( uint8_t *macAddr )
// {
//     ETH_Configuration( macAddr );
//     ETH_DMATxDescChainInit(DMATxDscrTab, MACTxBuf, ETH_TXBUFNB);
//     ETH_DMARxDescChainInit(DMARxDscrTab, MACRxBuf, ETH_RXBUFNB);
//     NVIC_EnableIRQ(ETH_IRQn);
//     NVIC_SetPriority(ETH_IRQn, 0);
// }

// void ethISR() {
// 	uint32_t int_sta;

//     int_sta = ETH->DMASR;
//     printf("int_sta: %08x\r\n", int_sta);
//     printf("DMAIER: %08x\r\n", ETH->DMAIER);
// }

// void LinkNegotiate_SwitchLightOnly() {
// 	uint16_t controlregstate = ETH_ReadPHYRegister(PHY_ADDRESS, 0x00);
//     controlregstate &= 0xEFFF;
//     ETH_WritePHYRegister(PHY_ADDRESS, 0x00, controlregstate);
// 	Delay_Ms(50);
// 	controlregstate = ETH_ReadPHYRegister(PHY_ADDRESS, 0x00);
//     controlregstate |= 0x0200;
//     ETH_WritePHYRegister(PHY_ADDRESS, 0x00, controlregstate);
// }

// void LinkNegotiate_Blinky() {
// 	uint16_t controlregstate = ETH_ReadPHYRegister(PHY_ADDRESS, 0x00);
//     controlregstate &= 0xEFFF;
//     ETH_WritePHYRegister(PHY_ADDRESS, 0x00, controlregstate);
// 	Delay_Ms(50);
// 	controlregstate = ETH_ReadPHYRegister(PHY_ADDRESS, 0x00);
//     controlregstate |= 0x0200;
//     ETH_WritePHYRegister(PHY_ADDRESS, 0x00, controlregstate);
// 	Delay_Ms(2000);
// 	controlregstate = ETH_ReadPHYRegister(PHY_ADDRESS, 0x00);
//     controlregstate |= 0x1000;
//     ETH_WritePHYRegister(PHY_ADDRESS, 0x00, controlregstate);
// 	Delay_Ms(100);
// 	controlregstate = ETH_ReadPHYRegister(PHY_ADDRESS, 0x00);
//     controlregstate |= 0x0200;
//     ETH_WritePHYRegister(PHY_ADDRESS, 0x00, controlregstate);
// }

// void LinkNegotiate_PurelyAuto() {
// 	uint16_t controlregstate = ETH_ReadPHYRegister(PHY_ADDRESS, 0x00);
// 	controlregstate |= 0x8000; // reset
// 	ETH_WritePHYRegister(PHY_ADDRESS, 0x00, controlregstate);
// 	Delay_Ms(2000);
//     // controlregstate |= 0x0200;
//     // ETH_WritePHYRegister(PHY_ADDRESS, 0x00, controlregstate);
// 	// Delay_Ms(2000);
// 	// controlregstate = ETH_ReadPHYRegister(PHY_ADDRESS, 0x00);
//     // controlregstate |= 0x1000;
//     // ETH_WritePHYRegister(PHY_ADDRESS, 0x00, controlregstate);
// 	// Delay_Ms(100);
// 	controlregstate = ETH_ReadPHYRegister(PHY_ADDRESS, 0x00);
//     controlregstate |= 0x0200;
//     ETH_WritePHYRegister(PHY_ADDRESS, 0x00, controlregstate);
// }

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
	// ResetPHY();
	RS485_Init();
	USART_Printf_Init(115200);	
	printf("SystemClk:%d\r\n",SystemCoreClock);
	printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
	printf("This is printf example\r\n");
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
    // test integer divide speed
    uint32_t num_divs_per_test = 0x1000;
    uint64_t dividend = 0x1234567812345678;
    uint32_t divisor = 0x99;
    uint64_t noopt_accumulator = 0;
    while (1) {
        uint32_t before = current_time_us();
        for (uint32_t i = 0; i < num_divs_per_test; i++) {
            // volatile uint64_t result = dividend / (divisor + i);
            // noopt_accumulator += result;
            current_time_us();
        }
        uint32_t after = current_time_us();
        printf("noopt_accumulator: %016llx\r\n", noopt_accumulator);
        printf("time is %d\r\n", current_time_ms());
        printf("time elapsed (us): %d\r\n", after - before);
        Delay_Ms(500);
    }

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

