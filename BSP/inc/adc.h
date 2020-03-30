#ifndef _ADC_H
#define _ADC_H



#define ADC_PORT        GPIOA
#define ADC_PIN         GPIO_PIN_1


#define AD_CHANNEL_MAX  8
#define AD_GROUP_MAX    50

void Adc_Config(void);

#endif