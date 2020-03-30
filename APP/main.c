#include "usart.h"
#include "type.h"
#include "modbus_asc.h"
//#include "FlashIf.h"
#include "stm32l0xx.h"
#include "para.h"
#include "ds18b20.h"
#include "algorithm.h"

#include "string.h"


#define THR(x) (x - 2731) * 10000  
#define TENM_FIL_NUM    10
extern UserTypeDef UserPara;

IWDG_HandleTypeDef  IWDG_HandleStructure;

//uint32_t TemValue;
uint32_t ReadDataStartTime;
FlagStatus ReadDataFlag = RESET;
FlagStatus StaChangeFlag = RESET;
int TemFilterBuf[TENM_FIL_NUM];
int TemFilterBufBak[TENM_FIL_NUM];
extern FlagStatus UartRecvFlag;

//系统默认时钟为MSI分频为2M，此处配置时钟为HSI16M(L031的时钟最大为16M)
static void SystemClock_Config(void)
{
    RCC_ClkInitTypeDef RCC_ClkInitStruct;

    __HAL_RCC_HSI_CONFIG(RCC_HSI_ON);
    __HAL_RCC_HSI_CALIBRATIONVALUE_ADJUST(RCC_HSICALIBRATION_DEFAULT);
    
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV4;                          
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;  
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;  
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0);
}


//看门狗时钟为LSI（32.768K）8分频而来，约为4K
static void User_Iwdg_Init(void)
{
    IWDG_HandleStructure.Init.Prescaler = IWDG_PRESCALER_8;
    IWDG_HandleStructure.Init.Reload = 0x0FA0;                                  //重装载值为4000，约为1S
    IWDG_HandleStructure.Init.Window = 0x0FA0;
    IWDG_HandleStructure.Instance = IWDG;
    HAL_IWDG_Init(&IWDG_HandleStructure);
}


static void User_Iwdg_Feed(void)
{
    HAL_IWDG_Refresh(&IWDG_HandleStructure);
}



void SysTick_Handler(void)
{
    HAL_IncTick();
}

 

void TemSta_Handle(void)
{
    if(ReadDataFlag == RESET)
    {
        ReadDataStartTime = HAL_GetTick();
    }
    else
    {
        if(HAL_GetTick() - ReadDataStartTime >= 160)
        { 
            ReadDataFlag = RESET;
            StaChangeFlag = RESET;
        }
    }   
}

 
static void TemData_Handle(void)
{
    static uint32_t TemCountCnt = 0;                                           //必须初始化为0
    static uint32_t ReadTemCnt = 0; 
    static FlagStatus FillBufStatus = RESET;
    static uint8_t temcnt=0;
    int PreTemp;

    //Disable_Lpuart1();                                                        //读温度时会关闭总中断，此时若有数据进来可能会导致串口死掉
    PreTemp = DS18B20_Get_Temp();                                               //每秒读一次温度值
    //Enable_Lpuart1();
    
    if(ReadTemCnt++ <= 2)                                                      //前3s的数据不要（因为会读出个85度）
    {
        return;
    }
        
    if(PreTemp == -27310000)                                                    //初始化18B20不成功,上电会读个85度上来，去掉    
    {
        DS18B20_Init();                                                         //出现故障时复位一下总线
        ReadTemCnt = 0;
        return;
    }
    
    if((PreTemp < -5000000) || (PreTemp > 13000000))                            //温度不在-50~130范围内丢掉
    {
        return;
    }
    if(UartRecvFlag == RESET)                                                   //在接收数据的过程中会产生串口中断
    {
        UserPara.Temp = PreTemp;                                                //影响读数据的准确性,所以在接收数据的过程中读取的数据丢掉
    }
    if(FillBufStatus == RESET)                                                  //开始的十几秒得到稳定数据再打开串口
    {
        TemFilterBuf[temcnt++] = UserPara.Temp;
        if(temcnt >= TENM_FIL_NUM)
        {
            memcpy((uint8_t*)TemFilterBufBak, (uint8_t*)TemFilterBuf, TENM_FIL_NUM * 4);
            UserPara.Temp = (int)GetDelExtremeAndAverage(TemFilterBufBak, TENM_FIL_NUM, TENM_FIL_NUM / 3u, TENM_FIL_NUM / 3u); 
            Enable_Lpuart1();
            FillBufStatus = SET;
        }
        else
        {
            return;
        }
    }
    memcpy((uint8_t*)TemFilterBuf, (uint8_t*)TemFilterBuf + 4, (TENM_FIL_NUM - 1) * 4);
    *(TemFilterBuf + (TENM_FIL_NUM - 1)) = UserPara.Temp;                       //得到最新值，滤波
    memcpy((uint8_t*)TemFilterBufBak, (uint8_t*)TemFilterBuf, TENM_FIL_NUM * 4);
    UserPara.Temp = (int)GetDelExtremeAndAverage(TemFilterBufBak, TENM_FIL_NUM, TENM_FIL_NUM / 3u, TENM_FIL_NUM / 3u);    
    
    if(UserPara.Temp >  THR(UserPara.Up_Thr))                                   //高于最高值
    {
        if(!(UserPara.AlarmSta & 0x00010000))                                   //从正常状态跳转到高于高值
        {
            if(TemCountCnt++ >= UserPara.Du_Thr - 1)                            //如果持续超过阀值时间
            {         
                TemCountCnt = 0;
                UserPara.AlarmSta |= 0x00010000;                                //报警
                UserPara.AlarmSta &= 0x00010000;
            }
        }
        else                                                                    //持续时间+1s
        {
            TemCountCnt = 0;                                                    //如果在阈值附近抖动，计数值重新开始
            UserPara.Duration += 1;
        }
    }
    else if(UserPara.Temp < THR(UserPara.Do_Thr))                               //低于最低值
    {
        if(!(UserPara.AlarmSta & 0x00000001))                                   //从正常状态跳转到低于低值
        {
            if(TemCountCnt++ >= UserPara.Du_Thr - 1)                            //如果持续超过阀值时间
            {
                TemCountCnt = 0;
                UserPara.AlarmSta |= 0x00000001;                                //报警
                UserPara.AlarmSta &= 0x00000001;
            }
        }
        else                         
        {
            TemCountCnt = 0;                                                    //如果在阈值附近抖动，计数值重新开始
            UserPara.Duration += 1;                                             //持续时间+1s
        }
    }
    else                                                                        //在阈值范围内
    {
        if(UserPara.AlarmSta != 0)                                              //从报警状态切换到正常状态
        {
            UserPara.Duration += 1;                                             //先要继续+1s
            if(TemCountCnt++ >= UserPara.Du_Thr - 1)                            //如果持续超过阀值时间
            {
                TemCountCnt = 0;
                UserPara.AlarmSta = 0;
                UserPara.Duration = 0;
            }
        }
        else
        {
            TemCountCnt = 0;                                                    //如果在阈值附近抖动，计数值重新开始
            UserPara.AlarmSta = 0;
            UserPara.Duration = 0;
        }
    }    
}




void main(void)
{ 
    uint32_t Alarm_Pre_Sta, Alarm_Cur_Sta;                                      //报警上次状态和本次状态
    uint32_t StartTime;
    
    SystemClock_Config();                                                       //系统时钟初始化为16M
    HAL_InitTick(1);                                                            //滴答时钟初始化
    Uart_Config_Init();                                                         //串口初始化
    ReadPara();                                                                 //初始化参数
    DS18B20_Init();                                                             //初始化18B20 IO
   // User_Iwdg_Init();                                                           //初始化独立看门狗
    Disable_Lpuart1();                                                          //先关闭串口，等数据稳定再打开
    while(1)
    {
      //  User_Iwdg_Feed();                                                       //喂狗
        TemSta_Handle();
        Alarm_Pre_Sta = UserPara.AlarmSta;                                      //获得上次的报警状态
        MBASC_Function();                                                       //modbus处理
        if(HAL_GetTick() - StartTime >= 100)
        {
            TemData_Handle();
            Alarm_Cur_Sta = UserPara.AlarmSta;                                  //获得本次报警状态
            if(Alarm_Cur_Sta != Alarm_Pre_Sta)                                  //两次报警状态不相同即发生状态变化
            {
                StaChangeFlag = SET;
            }
            StartTime = HAL_GetTick();            
        }
        if(HAL_GetTick() - StartTime >= 1000)
        {
             //UART1_SendData        
        }
    }
}





