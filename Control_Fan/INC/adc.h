/* adc.h */
#ifndef ADC_H_
#define ADC_H_

void ADC_Init(void);
void ADC_SelectChannel(unsigned char u8Channel);
void ADC_StartConv(void);
unsigned char ADC_IsBusy(void);
unsigned int ADC_GetData(void);
void ADC_ClearFlag(void);
void ADC_Enable(void);
void ADC_Disable(void);
void ADC_ISR(void);

#endif
