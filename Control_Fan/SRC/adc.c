/* adc.c */
#include <MS51.h>
#include "adc.h"

void ADC_Init(void)
{
	ADCCON1 |= (1 << 0);
	/* Clock */
	ADCCON1 &= ~(0x30);
	ADCCON1 |= 0x00;
	/* Channel 4 */
	P0M1 |= (1 << 5);
	P0M2 &= ~(1 << 5);
	AINDIDS |= (1 << 4);
	EADC = 1;
}

void ADC_SelectChannel(unsigned char u8Channel)
{
	ADCCON0 &= ~(0x0F);
	ADCCON0 |= (u8Channel & 0x07);
}

void ADC_StartConv(void)
{
	ADCS = 1;
}

unsigned char ADC_IsBusy(void)
{
	return ADCS;
}

unsigned int ADC_GetData(void)
{
	return (ADCRH * 16 + (ADCRL & 0x0F));
}

void ADC_ClearFlag(void)
{
	ADCF = 0;
}

void ADC_Enable(void)
{
	ADCCON1 |= (1 << 0);
}

void ADC_Disable(void)
{
	ADCCON1 &= ~(1 << 0);
}
