/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32v30x_it.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2022/01/18
* Description        : Main Interrupt Service Routines.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "main.h"
#include "ch32v30x_it.h"
#include "ch32v30x.h"

void ethISR(void)
{
    /* Minimal stub for the non-Ethernet example build. */
}

void NMI_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void HardFault_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
// void ETH_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void EXTI9_5_IRQHandler(void) __attribute__((interrupt()));
/*********************************************************************
 * @fn      NMI_Handler
 *
 * @brief   This function handles NMI exception.
 *
 * @return  none
 */
void NMI_Handler(void)
{
  while (1)
  {
  }
}

/*********************************************************************
 * @fn      HardFault_Handler
 *
 * @brief   This function handles Hard Fault exception.
 *
 * @return  none
 */
void HardFault_Handler(void)
{
    printf("HardFault_Handler\r\n");

    printf("mepc  :%08x\r\n", __get_MEPC());
    printf("mcause:%08x\r\n", __get_MCAUSE());
    printf("mtval :%08x\r\n", __get_MTVAL());
    NVIC_SystemReset();
    while(1);
}

/*********************************************************************
 * @fn      EXTI9_5_IRQHandler
 *
 * @brief   This function handles GPIO exception.
 *
 * @return  none
 */
void EXTI9_5_IRQHandler(void)
{
    printf("exti95\r\n");
    EXTI_ClearITPendingBit(EXTI_Line7);     /* Clear Flag */
}

/*********************************************************************
 * @fn      DMA1_Channel4_IRQHandler
 *
 * @brief   uart1 dma tx completion interrupt.
 *
 * @return  none
 */
void DMA1_Channel4_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA1_Channel4_IRQHandler(void)
{

    if(DMA_GetITStatus(DMA1_IT_TC4))
    {
    	// /* disable uart tx dma */
        // DMA_Cmd(DMA1_Channel4,DISABLE);
        // DMA_ClearITPendingBit(DMA1_IT_TC4);
        RS485_DMA_TC_Callback(USART1, 0);
    }

}

void USART1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void USART1_IRQHandler(void)
{
    debug_int_flag = 2;
    if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
    {
        // USART_ReceiveData(USART1); // clear IDLE flag
        // debug_int_flag = 1;
        RS485_USART_IDLE_Callback(&buffers, USART1, 0);
    }
    else if (USART_GetITStatus(USART1, USART_IT_TC) != RESET)
    {
        // USART_ClearITPendingBit(USART1, USART_IT_TC);
        // debug_int_flag = 3;
        RS485_USART_TC_Callback(&buffers, USART1, 0);
    }
}


// /*********************************************************************
//  * @fn      ETH_IRQHandler
//  *
//  * @brief   This function handles ETH exception.
//  *
//  * @return  none
//  */
// void ETH_IRQHandler(void)
// {
//     ethISR();
// }

// /*********************************************************************
//  * @fn      TIM2_IRQHandler
//  *
//  * @brief   This function handles TIM2 exception.
//  *
//  * @return  none
//  */
// void TIM2_IRQHandler(void)
// {
//     // WCHNET_TimeIsr(WCHNETTIMERPERIOD);
//     TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
// }
