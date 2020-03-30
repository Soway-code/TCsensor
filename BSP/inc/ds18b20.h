#ifndef __DS18B20_H
#define __DS18B20_H 

#include "stm32l0xx.h"

#define DS18B20_PORT                    GPIOA
#define DS18B20_PIN                     GPIO_PIN_9
#define DS18B20_IO_IN                   DS18B20_PORT->MODER &= ~(0x03 << 18)  //输入模式             
#define DS18B20_IO_OUT                  DS18B20_PORT->MODER |= (0x01 << 18)   //通用输出模式  
#define	DS18B20_DQ_OUT_H                DS18B20_PORT->BSRR  = DS18B20_PIN     //拉高  
#define	DS18B20_DQ_OUT_L                DS18B20_PORT->BRR  = DS18B20_PIN      //拉低  
#define	DS18B20_DQ_IN_READ              (DS18B20_PORT->IDR & DS18B20_PIN) ? GPIO_PIN_SET : GPIO_PIN_RESET//读IO电平

void Delay_Us(uint32_t cnt);
void Delay_Ms(uint32_t cnt);

void DS18B20_Init(void);			                                //初始化DS18B20	
int DS18B20_Get_Temp(void);	                                                //获取温度

#endif















