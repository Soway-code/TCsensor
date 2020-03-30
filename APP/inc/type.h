#ifndef _TYPE_H
#define _TYPE_H

#include "stm32l0xx.h"



typedef struct 
{
    int Temp;           //温度值*10000 
    uint8_t SlaveAddr;
    //uint8_t SenSta;     //传感器状态  用不到      
    uint16_t Up_Thr;    //温度上阀值  
    uint16_t Do_Thr;    //温度下阀值
    uint16_t Du_Thr;    //持续时间阀值（连续持续的时间才进行状态切换）   
    uint32_t Duration;  //超过上下温度阈值持续时间
    uint32_t AlarmSta;  //报警状态
}UserTypeDef;




#endif