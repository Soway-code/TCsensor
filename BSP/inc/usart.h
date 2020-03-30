#ifndef _USART1_H
#define _USART1_H

#include "stm32l0xx.h"
#include "stm32l031xx.h"

#define EN_485_PORT             GPIOA
#define EN_485_PIN              GPIO_PIN_4 
#define TX_ON                   EN_485_PORT->BSRR = EN_485_PIN
#define TX_OFF                  EN_485_PORT->BRR = EN_485_PIN
#define USART1_MAX_DATALEN      200    


#define LPUSART1_PORT           GPIOA
#define LPUSART1_RX             GPIO_PIN_2
#define LPUSART1_TX             GPIO_PIN_3

void Uart_Config_Init(void);
void Enable_Lpuart1(void);
void Disable_Lpuart1(void);
int32_t UART1_RecvData(uint8_t *UART1_RecvBuff, uint32_t Len);
uint32_t UART1_SendData(uint8_t *UART1_SendBuff, uint32_t Len);

void uprintf(const char *fmt,...);

#endif