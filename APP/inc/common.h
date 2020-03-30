/**
  ******************************************************************************
  * @file    STM32F0xx_IAP/inc/common.h 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    29-May-2012
  * @brief   Header for main.c module
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _COMMON_H
#define _COMMON_H

/* Includes ------------------------------------------------------------------*/
#include "stm32L0xx.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Constants used by Serial Command Line Mode */
//#define CMD_STRING_SIZE       128


/* Exported macro ------------------------------------------------------------*/
/* Common routines */
    
unsigned long BECharArrayToU32(unsigned char *pData);
void ClearMemory(unsigned char *pbuf, unsigned long num, unsigned char ucData);
void Decoding(unsigned char *pSource, unsigned long Length);

void TimingDelay_Decrement(void);
void TimingDelay_SetTime(unsigned long Delay);
unsigned long TimingDelay_TimeOut(void);

/* Exported functions ------------------------------------------------------- */

#endif  /* _COMMON_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
