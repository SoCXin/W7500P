#include "bsp_uart.h"

//	UART_InitTypeDef* UART_InitStruct
void UART_Configuration(void)
{
	UART_InitTypeDef UART_InitStructure;
  UART_InitStructure.UART_BaudRate = 115200;
  UART_InitStructure.UART_WordLength = UART_WordLength_8b ;
  UART_InitStructure.UART_StopBits = UART_StopBits_1;
  UART_InitStructure.UART_Parity = UART_Parity_No ;
  UART_InitStructure.UART_Mode = UART_Mode_Rx | UART_Mode_Tx;
  UART_InitStructure.UART_HardwareFlowControl = UART_HardwareFlowControl_None ;  
  UART_Init(UART0,&UART_InitStructure);
	
  UART_InitStructure.UART_BaudRate = 115200;
  UART_InitStructure.UART_WordLength = UART_WordLength_8b ;
  UART_InitStructure.UART_StopBits = UART_StopBits_1;
  UART_InitStructure.UART_Parity = UART_Parity_No ;
  UART_InitStructure.UART_Mode = UART_Mode_Rx | UART_Mode_Tx;
  UART_InitStructure.UART_HardwareFlowControl = UART_HardwareFlowControl_None ;  
  UART_Init(UART1,&UART_InitStructure);
//  UART_ITConfig(UART1,(UART_IT_FLAG_TXI|UART_IT_FLAG_RXI),ENABLE);    /* Configure Uart1 Interrupt Enable*/
//  NVIC_ClearPendingIRQ(UART1_IRQn);    
//  NVIC_EnableIRQ(UART1_IRQn);	 						/* NVIC configuration */
}

