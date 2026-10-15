#include "rs485.h"

GPIO_TypeDef* RS485_DE_Ports[] = {GPIOA}; // array of DE ports for each USART (USART1 maps to index 0), for now only USART1
uint16_t RS485_DE_Pins[] = {GPIO_Pin_8}; // array of DE pins for each USART (USART1 maps to index 0), for now only USART1
DMA_Channel_TypeDef* RS485_TX_DMA_Channels[] = {DMA1_Channel4}; // array of TX DMA channels for each USART (USART1 maps to index 0), for now only USART1
uint32_t RS485_TX_DMA_TC_IT_Flags[] = {DMA1_IT_TC4}; // array of TX DMA transfer complete interrupt flags for each USART (USART1 maps to index 0), for now only USART1
DMA_Channel_TypeDef* RS485_RX_DMA_Channels[] = {DMA1_Channel5}; // array of RX DMA channels for each USART (USART1 maps to index 0), for now only USART1

/**
 * @brief  Initialize GPIO for RS485 (for now USART1)
 */
void RS485_Pin_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    // USART1 TX: PA9, RX: PA10, DE: PA8
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/**
 * @brief  Initialize USART for RS485 (for now USART1)
 * @param  baudrate: The baud rate for USART communication
 */
void RS485_USART_Init(uint32_t baudrate) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    USART_InitTypeDef USART_InitStructure = {0};
    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);

    USART_Cmd(USART1, ENABLE);
}

/**
 * @brief  Initialize DMA for RS485 (for now USART1)
 * @param  buffers: Pointer to the RS485_Buffers_t structure containing TX and RX buffers and RX buffer length
 */
void RS485_DMA_Init(RS485_Buffers_t* buffers) {
    DMA_InitTypeDef DMA_InitStructure = {0};
    // UART1 TX: DMA1_Channel4 RX: DMA1_Channel5
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    DMA_DeInit(DMA1_Channel4);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART1->DATAR);
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)buffers->tx_buffers[0];
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	//DMA_InitStructure.DMA_BufferSize = size(TX_DMA_dataBuf);
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel4, &DMA_InitStructure);
    DMA_DeInit(DMA1_Channel5);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART1->DATAR);
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)buffers->rx_buffers[0];
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = buffers->rx_buffer_len;
	DMA_Init(DMA1_Channel5, &DMA_InitStructure);
    DMA_ClearFlag(DMA1_IT_TC4);

	DMA_Cmd(DMA1_Channel5, ENABLE);
	USART_DMACmd(USART1, USART_DMAReq_Rx | USART_DMAReq_Tx, ENABLE);
}

/**
 * @brief  Initialize interrupts for RS485 (for now USART1)
 */
void RS485_Interrupt_Init(void) {
    NVIC_InitTypeDef NVIC_InitStructure = {0};
    // UART1 TX: DMA1_Channel4 RX: DMA1_Channel5
    DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
    USART_ITConfig(USART1, USART_IT_TC, DISABLE); // gets enabled in interrupt

    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**
 * @brief  Initialize RS485 (for now USART1) by setting up pins, USART, DMA, and interrupts
 * @param  buffers: Pointer to the RS485_Buffers_t structure containing TX and RX buffers and RX buffer length
 * @param  baudrate: Baud rate for the USART
 */
void RS485_Init(RS485_Buffers_t* buffers, uint32_t baudrate) {
    RS485_Pin_Init();
    RS485_USART_Init(baudrate);
    RS485_DMA_Init(buffers);
    RS485_Interrupt_Init();
}

/**
 * @brief  Callback for transfer complete interrupt for DMA channels associated with RS485.
 * Enables USART TC interrupt for timing of RS485 DE switchoff after transmission complete.
 * Cannot disable DE here since DMA TC triggers after last byte is send to UART, not out UART.
 * @param  USARTx: Pointer to the USART peripheral
 * @param  USART_num: Index of the USART (e.g., 0 for USART1)
 */
void RS485_DMA_TC_Callback(USART_TypeDef *USARTx, uint8_t USART_num) {
    DMA_Cmd(RS485_TX_DMA_Channels[USART_num], DISABLE);
    DMA_ClearITPendingBit(RS485_TX_DMA_TC_IT_Flags[USART_num]);
    USART_ITConfig(USARTx, USART_IT_TC, ENABLE);
}

/**
 * @brief  Callback for transfer complete interrupt for USART associated with RS485.
 * Disables the USART TC interrupt, switches off the RS485 DE (Driver Enable) signal,
 * and enables RX DMA for response reception.
 * @param  USARTx: Pointer to the USART peripheral
 * @param  USART_num: Index of the USART (e.g., 0 for USART1)
 */
void RS485_USART_TC_Callback(RS485_Buffers_t* buffers, USART_TypeDef *USARTx, uint8_t USART_num) {
    DMA_Cmd(RS485_RX_DMA_Channels[USART_num], DISABLE);
    USART_ClearITPendingBit(USARTx, USART_IT_TC);
    USART_ITConfig(USARTx, USART_IT_TC, DISABLE);
    USART_ReceiveData(USARTx);
    USART_ITConfig(USARTx, USART_IT_IDLE, ENABLE);
    GPIO_WriteBit(RS485_DE_Ports[USART_num], RS485_DE_Pins[USART_num], Bit_RESET);
    DMA_SetCurrDataCounter(RS485_RX_DMA_Channels[USART_num], buffers->rx_buffer_len);
    RS485_RX_DMA_Channels[USART_num]->MADDR = (uint32_t)buffers->rx_buffers[USART_num];
    DMA_Cmd(RS485_RX_DMA_Channels[USART_num], ENABLE);
}

/**
 * @brief  Callback for idle line interrupt for USART associated with RS485.
 * Reads the received data to clear the interrupt flag.
 * @param  buffers: Pointer to the RS485_Buffers_t structure containing TX and RX buffers / status
 * @param  USARTx: Pointer to the USART peripheral
 * @param  USART_num: Index of the USART (e.g., 0 for USART1)
 */
void RS485_USART_IDLE_Callback(RS485_Buffers_t* buffers, USART_TypeDef *USARTx, uint8_t USART_num) {
    if (USART_GetITStatus(USARTx, USART_IT_IDLE)) {
        USART_ReceiveData(USARTx); // must read IT status and receive data to clear this interrupt flag
        USART_ITConfig(USARTx, USART_IT_IDLE, DISABLE);
        buffers->num_rx_bytes[USART_num] = buffers->rx_buffer_len - DMA_GetCurrDataCounter(RS485_RX_DMA_Channels[USART_num]);
    }
}