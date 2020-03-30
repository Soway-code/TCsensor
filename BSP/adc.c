#include "adc.h"
#include "stm32l0xx.h"

uint16_t g_usADC1ConvertedValue[AD_GROUP_MAX][AD_CHANNEL_MAX];
ADC_HandleTypeDef   AdcHandle;
DMA_HandleTypeDef   DmaHandle;


static void Adc_Gpio_Init(void)
{
    GPIO_InitTypeDef   GPIO_InitStruct;
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    GPIO_InitStruct.Pin = ADC_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(ADC_PORT, &GPIO_InitStruct);
}



static void Adc_Init(void)
{
    //uint32_t CalibrationFactor;
    ADC_ChannelConfTypeDef   sConfig;
    __HAL_RCC_ADC1_CLK_ENABLE();
    
    AdcHandle.Instance = ADC1;
    AdcHandle.Init.ClockPrescaler        = ADC_CLOCK_SYNC_PCLK_DIV4;    //时钟4分频    
    AdcHandle.Init.Resolution            = ADC_RESOLUTION_12B;          //12位精度          
    AdcHandle.Init.DataAlign             = ADC_DATAALIGN_RIGHT;         //右对齐          
    AdcHandle.Init.ScanConvMode          = ADC_SCAN_DIRECTION_FORWARD;  //扫描方向，向前扫描   
    AdcHandle.Init.EOCSelection          = ADC_EOC_SEQ_CONV;           
    AdcHandle.Init.LowPowerAutoPowerOff  = DISABLE;
    AdcHandle.Init.LowPowerFrequencyMode = DISABLE;
    AdcHandle.Init.LowPowerAutoWait      = DISABLE;                      
    AdcHandle.Init.ContinuousConvMode    = ENABLE;                      //连续转换模式                                                   
    AdcHandle.Init.DiscontinuousConvMode = DISABLE;                       
    AdcHandle.Init.ExternalTrigConv      = ADC_SOFTWARE_START;          //软件触发          
    AdcHandle.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE; 
    AdcHandle.Init.DMAContinuousRequests = ENABLE;                      //开启DMA请求                      
    AdcHandle.Init.Overrun               = ADC_OVR_DATA_OVERWRITTEN;      
    AdcHandle.Init.OversamplingMode      = DISABLE;                      
    AdcHandle.Init.SamplingTime          = ADC_SAMPLETIME_160CYCLES_5;   //采样周期为160.5个周期
    AdcHandle.DMA_Handle                 = &DmaHandle;                  //连接ADC和DMA        
    HAL_ADC_Init(&AdcHandle);
 /*       
    sConfig.Channel = ADC_CHANNEL_0;               
    sConfig.Rank = 0; 
    HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
    */
    sConfig.Channel = ADC_CHANNEL_1;               
    sConfig.Rank = 1; 
    HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
    /*
    sConfig.Channel = ADC_CHANNEL_2;               
    sConfig.Rank = 2; 
    HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);

    sConfig.Channel = ADC_CHANNEL_3;               
    sConfig.Rank = 3; */
    HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);

    HAL_ADCEx_Calibration_Start(&AdcHandle, ADC_SINGLE_ENDED);
    //CalibrationFactor = HAL_ADCEx_Calibration_GetValue(&AdcHandle, ADC_SINGLE_ENDED);
    //HAL_ADCEx_Calibration_SetValue(&AdcHandle, ADC_SINGLE_ENDED, CalibrationFactor);
    HAL_ADC_Start_DMA(&AdcHandle, (uint32_t*)&g_usADC1ConvertedValue, AD_CHANNEL_MAX * AD_GROUP_MAX);
}


static void Dma_Init(void)
{
    __HAL_RCC_DMA1_CLK_ENABLE();
    
    DmaHandle.Instance                 = DMA1_Channel1;
    DmaHandle.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    DmaHandle.Init.PeriphInc           = DMA_PINC_DISABLE;
    DmaHandle.Init.MemInc              = DMA_MINC_ENABLE;
    DmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    DmaHandle.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
    DmaHandle.Init.Mode                = DMA_CIRCULAR;
    DmaHandle.Init.Priority            = DMA_PRIORITY_HIGH;
    DmaHandle.Init.Request             = DMA_REQUEST_0;
    DmaHandle.Parent                   = &AdcHandle;                    //连接ADC和DMA  
    HAL_DMA_Init(&DmaHandle);
}


void Adc_Config(void)
{
    Adc_Gpio_Init();
    Dma_Init();
    Adc_Init();
}



