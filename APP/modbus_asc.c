#include "usart.h"
#include "modbus_asc.h"
#include "modbus_ascii.h"
#include "type.h"
#include "eeprom.h"
#include "para.h"
#include "DS18B20.h"

/************************************************************************************************************************************************************************
** 版权：   2016-2026, 深圳市信为科技发展有限公司
** 文件名:  modbus_asc.c
** 作者:    庄明群
** 版本:    V1.0.0
** 日期:    2016-09-05
** 描述:    modbus ascii 功能码
** 功能:         
*************************************************************************************************************************************************************************
** 修改者:          No
** 版本:  		
** 修改内容:    No 
** 日期:            No
*************************************************************************************************************************************************************************/

uint8_t SendLen;
uint8_t SendBuf[200];
uint32_t AlarmLastSta;
extern uint8_t UART1_RXBuff[USART1_MAX_DATALEN];
extern UserTypeDef UserPara;
extern uint8_t u8SendNum;
extern FlagStatus UartRecvFrameOK;
extern uint32_t TemValue;
extern FlagStatus ReadDataFlag;            
extern FlagStatus StaChangeFlag;
//**************************************************************************************************
// 名称         : MBASC_SendMsg()
// 创建日期     : 2016-09-05
// 作者         : 庄明群
// 功能         : 以ASCII格式发送消息
// 输入参数     : 帧缓存区(u8 *u8Msg),帧长(u8 u8MsgLen)
// 输出参数     : 无
// 返回结果     : 无
// 注意和说明   : 
// 修改内容     :
//**************************************************************************************************
extern uint8_t u8SendNum;

void MBASC_SendMsg(uint8_t *u8Msg, uint8_t u8MsgLen)
{
  
  Delay_Ms(50);
    if(MB_ADDRESS_BROADCAST != u8Msg[0])
    {
        MODBUS_ASCII_SendData(u8Msg, u8MsgLen);
    }
}

//**************************************************************************************************
// 名称         : MBASC_SendMsg_NoLimit()
// 创建日期     : 2016-09-05
// 作者         : 庄明群
// 功能         : 无限制性的以ASCII格式发送消息
// 输入参数     : 帧缓存区(u8 *u8Msg),帧长(u8 u8MsgLen)
// 输出参数     : 无
// 返回结果     : 无
// 注意和说明   : 
// 修改内容     :
//**************************************************************************************************
void MBASC_SendMsg_NoLimit(uint8_t *u8Msg, uint8_t u8MsgLen)
{
   
    Delay_Ms(50);
  MODBUS_ASCII_SendData(u8Msg, u8MsgLen);
}

//**************************************************************************************************
// 名称         : MBASC_SendErr()
// 创建日期     : 2016-09-05
// 作者         : 庄明群
// 功能         : 发送错误码的响应帧
// 输入参数     : 错误码(u8 ErrCode)
// 输出参数     : 无
// 返回结果     : 无
// 注意和说明   : 
// 修改内容     :
//**************************************************************************************************
void MBASC_SendErr(uint8_t ErrCode)
{
    SendLen = 1;
    SendBuf[SendLen++] |= 0x80;
    SendBuf[SendLen++] = ErrCode;
    MBASC_SendMsg_NoLimit(SendBuf, SendLen);
}

//**************************************************************************************************
// 名称         : MBASC_Fun03()
// 创建日期     : 2016-09-05
// 作者         : 庄明群
// 功能         : 03功能码，读单个寄存器操作
// 输入参数     : 无
// 输出参数     : 无
// 返回结果     : 无
// 注意和说明   : 
// 修改内容     :  1.将从机地址修改为0x45，对应的寄存器地址修改为0x45xx   2016.09.08
//                 2.修改从机地址后，对应的寄存地址可以变化，如：修改为0x46,则可以读取0x4630地址数据
//**************************************************************************************************
void MBASC_Fun03(void)
{
    uint8_t i;
    uint16_t Data_Buf;
    uint16_t Register_Num = 0;
    uint8_t ReadAdrH, ReadAdrL;
    
    ReadAdrH = UART1_RXBuff[2];
    ReadAdrL = UART1_RXBuff[3];

    Register_Num = ((uint16_t)UART1_RXBuff[4] << 8) + UART1_RXBuff[5];
     
    SendLen = 0;
    SendBuf[SendLen++] = UART1_RXBuff[0] ? UserPara.SlaveAddr : 0x00;
    SendBuf[SendLen++] = 0x03;	                        //功能码
    SendBuf[SendLen++] = Register_Num * 2;		                        //数据长度
                                                                                //如果读取范围溢出
    if ((!(((ReadAdrL >= MBASC_HOLDING_REG_REGION_BGEIN) && (ReadAdrL <= MBASC_HOLDING_REG_REGION_END)
        && (ReadAdrL + Register_Num <= (MBASC_HOLDING_REG_REGION_END + 1)))&& (0 != Register_Num)))
        || ((ReadAdrH != UserPara.SlaveAddr) && (ReadAdrH != MB_ADDRESS_BROADCAST)))
    {
        MBASC_SendErr(MB_EX_ILLEGAL_DATA_ADDRESS);
        return;
    }

    for (uint8_t k = 0; k < Register_Num; ReadAdrL++, k++)
    {
        switch (ReadAdrL)
        {
          case 0x30:
            Data_Buf = UserPara.SlaveAddr;				        //设备地址
            break;

          case 0x31:
            Data_Buf = 3;				        
            break;

          case 0x32:
            Data_Buf = 3;			                        
            break;

          case 0x33:
            Data_Buf = 0;						        
            break;

          case 0x34:
            Data_Buf = 1;	                        
            break;

          case 0x35:                                                          
            Data_Buf = 2;			                                             
            break;

          case 0x36:
            Data_Buf =0;		               
            break;

          case 0x37:
            Data_Buf = 0;                       
            break;

          case 0x38:
            Data_Buf = 0;                        
            break;

          case 0x39:
            Data_Buf = 0;	                                                                 
            break;

          case 0x3A:
            Data_Buf = 0;	                                                             
            break;  
            
          case 0x3B:
            Data_Buf = 0;	                                                             
            break;             
            
          case 0x3C:
            Data_Buf = 0;	                                                             
            break;
            
          case 0x3D:
            Data_Buf = 0;				  	                                                             
            break;             
            
          case 0x3E:
            Data_Buf = 0;	                                                             
            break;
            
          case 0x3F:
            Data_Buf = 0;                                                  
            break;
            
          case 0x40:
            Data_Buf = UserPara.Up_Thr;	//温度报警上阈值	        
            break;

          case 0x41:                                                        
            Data_Buf = UserPara.Do_Thr; //温度报警下阈值
            break;
            
          case 0x42:
            Data_Buf = UserPara.Du_Thr; //超出时间阈值                                
            break; 
            
          case 0x43:
            Data_Buf = 0;
            break;  
                
          case 0x44:
            Data_Buf = 0;
            break;
                
          case 0x45:
            Data_Buf = 0;
            break;
                
          case 0x46:
            Data_Buf = 0;
            break;
                
          case 0x47:
            Data_Buf = 0;
            break;
                
          case 0x48:
            Data_Buf = 0;
            break;
                
          case 0x49:
            Data_Buf = 0;
            break;
                
          case 0x4A:
            Data_Buf = 0;
            break;
                
          case 0x4B:
            Data_Buf = 0;
            break;
                
          case 0x4C:
            Data_Buf = 0;
            break;
                
          case 0x4D:
            Data_Buf = 0;
            break;
                
          case 0x4E:
            Data_Buf = 0;
            break;
                
          case 0x4F:
            Data_Buf = 0;
            break; 
            
          default:
            Data_Buf = 0;
            break;
        }
        for (i = 2; i > 0; i--)
        {
            SendBuf[SendLen++] = (uint8_t)(Data_Buf >> ((i - 1) * 8));
        }
    }
    MBASC_SendMsg_NoLimit(SendBuf, SendLen);                                      
}



//**************************************************************************************************
// 名称         : MBASC_Fun04()
// 创建日期     : 2016-09-05
// 作者         : 庄明群
// 功能         : 04功能码，读双个寄存器操作
// 输入参数     : 无
// 输出参数     : 无
// 返回结果     : 无
// 注意和说明   : 
// 修改内容     :  按照协议修改数据发送方式,发送浮点数据改为发送整形     2016.09.12
//**************************************************************************************************
void MBASC_Fun04(void)	
{
    uint8_t i;
    uint32_t Data_Buf;
    uint16_t Register_Num ;
    uint8_t ReadAdrH, ReadAdrL;
    
    ReadAdrH = UART1_RXBuff[2];
    ReadAdrL = UART1_RXBuff[3];
    
    Register_Num = ((uint16_t)UART1_RXBuff[4] <<8) + UART1_RXBuff[5];
  
    SendLen = 0;
    SendBuf[SendLen++] = UART1_RXBuff[0] ? UserPara.SlaveAddr : 0x00;
    SendBuf[SendLen++] = 0x04;
    SendBuf[SendLen++] = Register_Num * 2;		                        //数据长度

    if((!((((ReadAdrL <= MBASC_INPUT_REG_REGION_END) && ((ReadAdrL + Register_Num) <= (MBASC_INPUT_REG_REGION_END + 2))))
    && ((0 != Register_Num) && (0 == (Register_Num & 0x01)) && (0 == (ReadAdrL & 0x01)))))
    || (ReadAdrH != UserPara.SlaveAddr))   
    {
        MBASC_SendErr(MB_EX_ILLEGAL_DATA_ADDRESS);
        return;
    }   
    
    for (uint8_t k = 0; k < Register_Num; ReadAdrL += 2, k += 2)
    {
        switch (ReadAdrL)
        {
          case 0x00:
            Data_Buf = (uint32_t)((UserPara.Temp + 27310000) / 10000);
            if((StaChangeFlag == SET) && (UserPara.AlarmSta != 0))              //发生报警，有重要数据
            {
                Data_Buf |= 0x80000000;                                       //重要数据bit31置位，保持1600ms
                ReadDataFlag = SET;
            }
            /*if(StaChangeFlag == RESET)
            {
                Data_Buf &= 0x7FFFFFFF;
            }*/
            break;

          case 0x02:                                                                               
            Data_Buf = UserPara.Duration;	                                        
            break;

          case 0x04: 
            Data_Buf = UserPara.AlarmSta;
            //AlarmLastSta = UserPara.AlarmSta;
            break;
            
          default:
            Data_Buf = 0;
            break;
        }
        for(i = 4; i > 0; i--)
        {
            SendBuf[SendLen++] = (uint8_t)(Data_Buf >> ((i - 1) * 8));
        }
    }
    

    MBASC_SendMsg(SendBuf, SendLen);
}




//**************************************************************************************************
// 名称         : MBASC_Fun10()
// 创建日期     : 2016-09-05
// 作者         : 庄明群
// 功能         : 10功能码，写多个寄存器操作，用于修改参数
// 输入参数     : 无
// 输出参数     : 无
// 返回结果     : 无
// 注意和说明   : 
// 修改内容     : 1.先接收自动上传时间编码，再把编码转换成时间，解决发指令修改编码不重启后自动上传时
//                间不是想要的时间问题      2016.09.10
//                2.增加串口初始化（修改波特率后可以不重启单片机）
//**************************************************************************************************
void MBASC_Fun10()
{
    uint32_t index = 0;
    uint16_t Register_Num = 0;
    uint8_t ReadAdrH, ReadAdrL;
    
    ReadAdrH = UART1_RXBuff[2];
    ReadAdrL = UART1_RXBuff[3];
        
    Register_Num = ((uint16_t)UART1_RXBuff[4] << 8) + UART1_RXBuff[5]; 
    
    SendLen = 0;
    SendBuf[SendLen++] = UART1_RXBuff[0] ? UserPara.SlaveAddr : 0x00;
    SendBuf[SendLen++] = 0x10;
    SendBuf[SendLen++] = Register_Num * 2;
                                                                                //如果读取范围溢出
    if ((!((((ReadAdrL >= MBASC_MUL_REG_REGION_BGEIN) && (ReadAdrL <= MBASC_MUL_REG_REGION_END)
            && (ReadAdrL + Register_Num <= (MBASC_MUL_REG_REGION_END + 1)))
            || (ReadAdrL == 0x70))
            && ((0 != Register_Num)) && ((Register_Num * 2) == UART1_RXBuff[6])))
            || ((ReadAdrH != UserPara.SlaveAddr) && (ReadAdrH != MB_ADDRESS_BROADCAST)))
    {
        MBASC_SendErr(MB_EX_ILLEGAL_DATA_ADDRESS);
        return;
    }

    for (uint8_t k = 0; k < Register_Num; ReadAdrL++, k++)
    {
        switch (ReadAdrL)
        {
            case 0x30:						                //设备地址
                UserPara.SlaveAddr = ((uint16_t)UART1_RXBuff[7+index] << 8) + UART1_RXBuff[8+index];
                if(UserPara.SlaveAddr >= 33 && UserPara.SlaveAddr <= 37)
                {
                    Eeprom_WriteNBytes(SLAVE_ADDR, &UserPara.SlaveAddr , 1);
                }
                break;

            case 0x31:						                  
                break;

            case 0x32:						             
                break;

            case 0x33:                                                             
                break;

            case 0x34:                                                              
                break;

            case 0x35:						                
                break;

            case 0x36:                                                              
                break;

            case 0x37:                                                           
                break;

            case 0x38:
                break;

            case 0x39:						                
                break;

            case 0x3A:
                break; 

            case 0x3B:						                
                break;

            case 0x3C:						                
                break;

            case 0x3D:						             
                break;

            case 0x3E:						                
                break; 

            case 0x3F:						               
                break;       

            case 0x40:
                UserPara.Up_Thr = ((uint16_t)UART1_RXBuff[7+index] << 8) + UART1_RXBuff[8+index];
                Eeprom_WriteNBytes(TEM_UP_THR, (uint8_t*)&UserPara.Up_Thr, 2);
                break; 

            case 0x41:
                UserPara.Do_Thr = ((uint16_t)UART1_RXBuff[7+index] << 8) + UART1_RXBuff[8+index];
                Eeprom_WriteNBytes(TEM_DO_THR, (uint8_t*)&UserPara.Do_Thr, 2);
                break;

            case 0x42:
                UserPara.Du_Thr = ((uint16_t)UART1_RXBuff[7+index] << 8) + UART1_RXBuff[8+index];
                Eeprom_WriteNBytes(TEM_DU_THR, (uint8_t*)&UserPara.Du_Thr, 2);
                break;                    

            case 0x43:						               
                break;  
                
            case 0x44:						               
                break;
                
            case 0x45:						               
                break;
                
            case 0x46:						               
                break;
                
            case 0x47:						               
                break;
                
            case 0x48:						               
                break;
                
            case 0x49:						               
                break;
                
            case 0x4A:						               
                break;
                
            case 0x4B:						               
                break;
                
            case 0x4C:						               
                break;
                
            case 0x4D:						               
                break;
                
            case 0x4E:						               
                break;
                
            case 0x4F:						               
                break;                

            case 0x70:						               
                break; 
                
            default:
                break;
        }
        index += 2;
    }
    
    MBASC_SendMsg(UART1_RXBuff, 6);
}





                
//**************************************************************************************************
// 名称         : MBASC_Fun41()
// 创建日期     : 2016-09-19
// 作者         : 梅梦醒
// 功能         : 重启系统（从boot开始执行）
// 输入参数     : 无
// 输出参数     : 无
// 返回结果     : 无
// 注意和说明   : 
// 修改内容     :
//**************************************************************************************************
void MBASC_Fun41(void)
{
    uint16_t ReadAdr = 0;
    uint16_t DataLength = 0;
    uint8_t ReadData;
      
    ReadAdr = ((uint16_t)(UART1_RXBuff[2]<< 8)) + UART1_RXBuff[3];
    DataLength = ((uint16_t)(UART1_RXBuff[4]<< 8)) + UART1_RXBuff[5];
        
    SendLen = 0;
    SendBuf[SendLen++] = UART1_RXBuff[0] ? UserPara.SlaveAddr : MB_ADDRESS_BROADCAST;
    SendBuf[SendLen++] = 0x41;
    for(uint8_t i = 2; i < 6; i++)
    {
        SendBuf[SendLen++] = UART1_RXBuff[i];
    }
                                                    //防止广播地址使程序进入boot
    if((0x0001 != ReadAdr) || (0 != DataLength) || (UART1_RXBuff[0] == MB_ADDRESS_BROADCAST))
    {
        return;
    }
    else 
    {
        Eeprom_WriteNBytes(1023,"\x0C", 1);
        Eeprom_ReadNBytes(1023, &ReadData, 1);
        if(ReadData == 0x0C)
        {
            SendBuf[SendLen++] = 0x00;
            MBASC_SendMsg(SendBuf, SendLen);
            while(0 != u8SendNum);                                              //等待数据发送完毕，此句要加上，否则执行此功能码是无响应
            NVIC_SystemReset();
        }
        else
        {
            SendBuf[SendLen++] = 0x01;
            MBASC_SendMsg(SendBuf, SendLen); 
        }
    } 
}     
     

//**************************************************************************************************
// 名称         : MBASC_Function()
// 创建日期     : 2016-09-05
// 作者         : 庄明群
// 功能         : modbus ascii通信处理
// 输入参数     : 无
// 输出参数     : 无
// 返回结果     : 无
// 注意和说明   : 
// 修改内容     :  
//                                         
//**************************************************************************************************
void MBASC_Function(void)
{
    uint16_t RecvLen = 0;
    if(UartRecvFrameOK == SET)
    {
        if(2 == MODBUS_ASCII_RecvData(UART1_RXBuff, &RecvLen))//校验错误
        {
            return;
        }  
        if(RecvLen <= 0)
        {
            return;                                                                 //没有接受到数据，无响应
        }

        else if((UserPara.SlaveAddr != UART1_RXBuff[0]) && (MB_ADDRESS_BROADCAST != UART1_RXBuff[0]))
        {
            return;                                                                 //接收的从机地址不对应，无响应
        }

        else
        {
            switch (UART1_RXBuff[1])
            {
              case 0x03:
                MBASC_Fun03();	                                                //读单个寄存器（单字节数据）
                break;

              case 0x04:
                MBASC_Fun04();	                                                //读双个寄存器（浮点数据）
                break;
                
              case 0x10:
                MBASC_Fun10();                                                  //写多个寄存器
                break;               
     
              case 0x41:
                MBASC_Fun41();
                break; 
                
              default:
                SendLen = 0;
                SendBuf[SendLen++] = UART1_RXBuff[0];
                SendBuf[SendLen++] = 0x80 | UART1_RXBuff[1];
                SendBuf[SendLen++] = MB_EX_ILLEGAL_FUNCTION;
                MBASC_SendMsg(SendBuf, SendLen);
                break;
             }
         }
    }
}
