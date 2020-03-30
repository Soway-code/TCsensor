#include <stdarg.h>
#include <string.h>
#include "type.h"
#include "usart.h"
#include "modbus_ascii.h"

#define LPUART1_ENABLE_IT(__HANDLE__, __INTERRUPT__) (((((uint8_t)(__INTERRUPT__)) >> 5U) == 1U)? ((__HANDLE__)->CR1 |= (1U << ((__INTERRUPT__) & UART_IT_MASK))): \
                                                           ((((uint8_t)(__INTERRUPT__)) >> 5U) == 2U)? ((__HANDLE__)->CR2 |= (1U << ((__INTERRUPT__) & UART_IT_MASK))): \
                                                           ((__HANDLE__)->CR3 |= (1U << ((__INTERRUPT__) & UART_IT_MASK))))
#define LPUART1_DISABLE_IT(__HANDLE__, __INTERRUPT__) (((((uint8_t)(__INTERRUPT__)) >> 5U) == 1U)? ((__HANDLE__)->CR1 &= ~ (1U << ((__INTERRUPT__) & UART_IT_MASK))): \
                                                           ((((uint8_t)(__INTERRUPT__)) >> 5U) == 2U)? ((__HANDLE__)->CR2 &= ~ (1U << ((__INTERRUPT__) & UART_IT_MASK))): \
                                                           ((__HANDLE__)->CR3 &= ~ (1U << ((__INTERRUPT__) & UART_IT_MASK))))


uint8_t u8SendNum = 0;                  //当前发送数据个数
uint8_t u8SendIndex = 0;                //发送数据缓冲前缀
uint8_t UART1_RXBuffLen = 0;            //接收数据缓存长度

FlagStatus UartRecvFlag = RESET;
FlagStatus UartRecvFrameOK = RESET;  //接收数据完成标志
uint8_t UART1_RXBuff[USART1_MAX_DATALEN];//接收数据缓存
uint8_t UART1_TXBuff[USART1_MAX_DATALEN];//发送数据缓存


//****************************************************************************************************************************************************************
// 名称               : Usart_Gpio_Init()
// 创建日期           : 2017-11-27
// 作者               : 梅梦醒
// 功能               : 通用串口收发引脚配置
// 输入参数           : 无
// 输出参数           : 无
// 返回结果           : 无
// 注意和说明         : 无
// 修改内容           :
//****************************************************************************************************************************************************************
void Usart_Gpio_Init(void)
{
    SET_BIT(RCC->IOPENR, RCC_IOPENR_GPIOAEN);                                   //GPIOA开时钟
    EN_485_PORT->MODER &= 0xFFFFFCFF;            
    EN_485_PORT->MODER |= 0x00000100;                                           //PA4通用输出模式
    EN_485_PORT->OTYPER &= ~(1<<4);                                             //推挽输出
    EN_485_PORT->OSPEEDR |= (3<<8);                                             //高速  
    EN_485_PORT->BRR = EN_485_PIN;                                              //输入状态为低
    
    LPUSART1_PORT->MODER &= 0xFFFFFF0F;            
    LPUSART1_PORT->MODER |= 0x000000A0;                                         //PA2 PA3复用功能模式
    LPUSART1_PORT->OTYPER &= ~(3<<2);                                           //推挽输出
    LPUSART1_PORT->AFR[0] &= 0xFFFF00FF;                                        //AF6，看手册
    LPUSART1_PORT->AFR[0] |= 0x00006600;
}






//****************************************************************************************************************************************************************
// 名称               : Usart_Config_Init()
// 创建日期           : 2017-11-27
// 作者               : 梅梦醒
// 功能               : 通用串口参数初始化
// 输入参数           : UARTx_ConfigTypeDef(串口通信参数)
// 输出参数           : 无
// 返回结果           : 无
// 注意和说明         : 无
// 修改内容           :
//****************************************************************************************************************************************************************
void Uart_Config_Init(void)
{  
    Usart_Gpio_Init(); 
      
    SET_BIT(RCC->APB1ENR, (RCC_APB1ENR_LPUART1EN));                             //开LPUSART1时钟 
    NVIC->IP[_IP_IDX(LPUART1_IRQn)] = (NVIC->IP[_IP_IDX(LPUART1_IRQn)] & ~(0xFF << _BIT_SHIFT(LPUART1_IRQn))) |
       (((1 << (8 - __NVIC_PRIO_BITS)) & 0xFF) << _BIT_SHIFT(LPUART1_IRQn));    //设置子优先级为1
    NVIC->ISER[0] = (1 << ((uint32_t)(LPUART1_IRQn) & 0x1F));                   //使能总中断
    
    LPUART1->CR1 &=  ~USART_CR1_UE;                                             //先失能串口
    MODIFY_REG(LPUART1->CR1, ((0x10001U << 12) | (1 << 10) | (1 << 9) | (1 << 3) | (1 << 2) | (1 << 15)), 
        UART_WORDLENGTH_8B | UART_PARITY_NONE | UART_MODE_TX_RX);               //配置数据位，校验位，串口模式
    MODIFY_REG(LPUART1->CR2, USART_CR2_STOP, UART_STOPBITS_1);                  //配置停止位
    MODIFY_REG(LPUART1->CR3, (USART_CR3_RTSE | USART_CR3_CTSE | USART_CR3_ONEBIT), UART_HWCONTROL_NONE);
    LPUART1->BRR = (uint32_t)(UART_DIV_LPUART(SystemCoreClock, 9600));          //配置波特率（系统时钟16M，波特率9600）
    CLEAR_BIT(LPUART1->CR2, (USART_CR2_LINEN | USART_CR2_CLKEN));
    CLEAR_BIT(LPUART1->CR3, (USART_CR3_SCEN | USART_CR3_HDSEL | USART_CR3_IREN));
    LPUART1_ENABLE_IT(LPUART1, UART_IT_RXNE);                                   //使能接收中断
    LPUART1->CR1 |=  USART_CR1_UE;                                              //使能串口
}





//******************************************************************************
// 名称               : UART1_RecvData()
// 创建日期           : 2017-11-27
// 作者               : 梅梦醒
// 功能               : UART1接收数据函数
// 输入参数           : uint8_t *UART1_RecvBuff 接收数据缓存
//                      uint32_t Len            接收数据长度        
// 输出参数           : 无
// 返回结果           : 无
// 注意和说明         : 
// 修改内容           :
//******************************************************************************
int32_t UART1_RecvData(uint8_t *UART1_RecvBuff, uint32_t Len)
{
    uint32_t i = 0;

    if ((0 == Len) || (((uint8_t*)NULL) == UART1_RecvBuff))
    {
        return 0;
    }

    if ((RESET == UartRecvFrameOK) || (0 == UART1_RXBuffLen))
    {
        return 0;
    }

    if (Len < UART1_RXBuffLen)
    {
        return -1;
    }

    Len = UART1_RXBuffLen;

    for (i = 0; i < Len; i++)
    {
        UART1_RecvBuff[i] = UART1_RXBuff[i];
    }

    UART1_RXBuffLen = 0;

    return Len;
}


//******************************************************************************
// 名称               : UART1_SendData()
// 创建日期           : 2017-11-27
// 作者               : 梅梦醒
// 功能               : UART1发送数据函数
// 输入参数           : uint8_t *UART1_SendBuff 发送数据缓存
//                      uint32_t Len            发送数据长度        
// 输出参数           : 无
// 返回结果           : 无
// 注意和说明         : 
// 修改内容           :
//******************************************************************************
/*
uint32_t UART1_SendData(uint8_t *UART1_SendBuff, uint32_t Len)
{
    uint32_t i = 0;

    if ((0 == Len) || (((uint8_t*)0) == UART1_SendBuff))
    {
        return 0;
    }
    if (u8SendNum != 0)
    {
        return 0;
    }

    if (Len > (sizeof(UART1_TXBuff) / sizeof(UART1_TXBuff[0])))
    {
        Len = (sizeof(UART1_TXBuff) / sizeof(UART1_TXBuff[0]));
    }

    for (i = 0; i < Len; i++)
    {
        UART1_TXBuff[i] = UART1_SendBuff[i];
    }
    TX_ON;
    LPUART1->TDR = UART1_TXBuff[0];
    u8SendIndex = 1;
    u8SendNum = Len;
    LPUART1_ENABLE_IT(LPUART1, UART_IT_TC);
    return Len;
}
*/
//******************************************************************************
// 名称               : UART1_SendData()
// 创建日期           : 2018-10-09
// 作者               : 梅梦醒
// 功能               : UART1发送数据函数
// 输入参数           : uint8_t *UART1_SendBuff 发送数据缓存
//                      uint32_t Len            发送数据长度        
// 输出参数           : 无
// 返回结果           : 无
// 注意和说明         : 使用中断发送，在读取温度关闭总中断时可能会引起串口中断无法再开启
// 修改内容           : 
//******************************************************************************
uint32_t UART1_SendData(uint8_t *buf, uint32_t len)
{
    uint8_t i;
    uint32_t TimeOutCount;
    
    if((0 == len) || (((uint8_t*)NULL) == buf) || (len > sizeof(UART1_TXBuff) / sizeof(UART1_TXBuff[0])))
    {
        return 0;
    }
    TX_ON;
    for(i = 0; i <= len; i++)                                                   //注意i<=len,否则会少发最后一个字节     
    {   
        TimeOutCount = 0;
        *(UART1_TXBuff + i) = *(buf + i);
        LPUART1->TDR = *(UART1_TXBuff + i);
        while((LPUART1->ISR & ((uint32_t)1U << (UART_IT_TXE >> 0x08U))) == RESET)
        {
            if(TimeOutCount++ > 0xffff)                                         //超时
            {
                TX_OFF;
                TimeOutCount = 0;
                return 0;
            }
        }
    } 
    TX_OFF;
    return len;
}




void Enable_Lpuart1(void)
{
    //LPUART1_ENABLE_IT(LPUART1, UART_IT_TC);
    LPUART1_ENABLE_IT(LPUART1, UART_IT_RXNE);
    LPUART1->CR1 |=  USART_CR1_UE;
}


void Disable_Lpuart1(void)
{
    //LPUART1_DISABLE_IT(LPUART1, UART_IT_TC);
    LPUART1_DISABLE_IT(LPUART1, UART_IT_RXNE);   
    LPUART1->CR1 &=  ~USART_CR1_UE;
}

//******************************************************************************
// 名称               : USART1_IRQHandler()
// 创建日期           : 2017-11-27
// 作者               : 梅梦醒
// 功能               : UART1发送中断
// 输入参数           : 无
//                      无       
// 输出参数           : 无
// 返回结果           : 无
// 注意和说明         : 
// 修改内容           :
//*******************************************************************************
void LPUART1_IRQHandler(void)
{
    uint8_t RecvByteData;
    
    UartRecvFlag = SET;                                                         //产生了串口中断，开始接收数据了，此时读取的温度数据丢弃
    if((LPUART1->ISR & ((uint32_t)1U << (UART_IT_RXNE >> 0x08U))) != RESET)
    {
        LPUART1->ICR = UART_IT_RXNE;
        
        RecvByteData = LPUART1->RDR;
                  
        MODBUS_ASCII_HandlRevData(RecvByteData); //接收ASCII数据     
    }
/*      
    if((LPUART1->ISR & ((uint32_t)1U << (UART_IT_TC >> 0x08U))) != RESET)       //中断发送易在读取温度时串口死掉
    {
        LPUART1->ICR = UART_IT_TC;

        if(u8SendIndex >= u8SendNum)
        {
            u8SendNum = 0;
            LPUART1_DISABLE_IT(LPUART1, UART_IT_TC);
            TX_OFF;
        }
        else
        {
            LPUART1->TDR = UART1_TXBuff[u8SendIndex++];
        }
    } */   
}


//**************************************************************************************************
// 名称         : uprintf(const char *fmt,...)
// 创建日期     : 2016-09-08
// 作者         : 梅梦醒
// 功能         : 向串口发送指定格式的数据
// 输入参数     : 入：fmt,...    发送数据指针
// 输出参数     : 无
// 返回结果     : 无
// 注意和说明   : 用usart打印串口的数据，用于调试
// 修改内容     : 
//**************************************************************************************************

void uprintf(const char *fmt,...)
{
    va_list marker;
    char buff[32];
    memset(buff,0,sizeof(buff));
    va_start(marker, fmt);
    vsprintf(buff,fmt,marker);
    va_end(marker);
    UART1_SendData((uint8_t*)buff, strlen(buff));
    while(u8SendNum!=0);    
}

