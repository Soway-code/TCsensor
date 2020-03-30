/**
  ******************************************************************************
  * @file    STM32F0xx_IAP/src/common.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    29-May-2012
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "common.h"

unsigned long TimingDelay = 0;


void TimingDelay_SetTime(unsigned long Delay)
{
    TimingDelay = Delay;
}

void TimingDelay_Decrement(void)
{
    if (TimingDelay != 0x00)
    { 
        TimingDelay--;
    }
}

unsigned long TimingDelay_TimeOut(void)
{
    return (0 == TimingDelay) ? 1 : 0;       
}


    
void SysTick_Handler(void)
{
    TimingDelay_Decrement();
    HAL_IncTick();
}
/*
unsigned long BECharArrayToU32(unsigned char *pData)
{
    unsigned long res = 0;
    res = pData[0];
    res <<= 8;
    res += pData[1];
    res <<= 8;
    res += pData[2];
    res <<= 8;
    res += pData[3];
    
    return res;
}

void ClearMemory(unsigned char *pbuf, unsigned long num, unsigned char ucData)
{
    for(long i = 0; i < num; i++)
    {
        pbuf[i] = ucData;
    }
}
*/
uint8_t encryptionString[64] = "ShenZhen Soway Science and Technology Development Co., Ltd.    ";

void Decoding(unsigned char *pSource, unsigned long Length)
{
    uint32_t DecodeIndex = 0;
    uint8_t key;
    uint32_t i;
    
    for(i = 0; i < Length; i++)
    {
        key = encryptionString[DecodeIndex];
        key += 11; 
        
        DecodeIndex += 1;
        if(DecodeIndex >= sizeof(encryptionString))
        {
            DecodeIndex = 0;
        }
        
        pSource[i] ^= key;
    }
}

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
