#include "main.h"
#include "overcurrent.h"
#include "logo.h"
#include "num.h"
/*------------------------- HW-------------------------*/
void HW_init(void)
{
	EXT0_config();
	InitPWM();
	set_PWMRUN;
	
	P11_PushPull_Mode;    // LED
	P11 = 1;
	
	P10_PushPull_Mode;   // BUZZ
	P10 = 0;
	
	P15_Input_Mode;      // CONTINUOUS PIN
	P12_Input_Mode;      // PEDAL PIN
	P03_Input_Mode;			 // ENC PIN
	
	Enable_ADC_AIN2; 

	set_EADC;
	EA = 1;          //Enable global interrupt
	
	InitTimer1();
	
	OLED_Init();
	OLED_DrawBMP(0,0,128,8,logo);
	Timer0_Delay1ms(2000);
	OLED_Clear();
	Timer0_Delay1ms(100);
	//num_chuc(0);
	num_donvi(5);
	OLED_DrawBMP(56,5,64,6,dot);
	num_float(0);

	clr_LOAD;
}
void InitTimer1(void)
{   TMOD |= (1 << 4);
	
    clr_T1M;        /* T1M=0, Timer0 Clock = Fsys/12 */
 	TMOD &= ~(1 << 5);
    
    /* Calculate the timer counter value for controlling the interrupt period on 10ms. */
    TL1 = LOBYTE(TIMER_DIV12_VALUE_1ms);	//Find  define in "Function_define.h" "TIMER VALUE"
    TH1 = HIBYTE(TIMER_DIV12_VALUE_1ms);

    set_ET1;        /* enable Timer0 interrupt */
		set_EA;
    set_TR1;        /* Timer0 start */
}
/*------------------------- ETX0-------------------------*/
void EXT0_config(void)
{
	P17_Input_Mode;	
	P30_Input_Mode;	
	set_P3S_0;
	set_IT0;
	set_EX0;
  set_EA;	
}
/*--------------------------PWM----------------------------*/	
void InitPWM(void)
{
    /* Initialize the pwm mode and clock. */
		P04_PushPull_Mode;
    PWM3_P04_OUTPUT_ENABLE;
   /*PMW clock source select define*/
		PWM_CLOCK_DIV_4;
		/*PWM type define*/
		PWM_EDGE_TYPE;
		/*clear PWM 12bit counter*/
		set_CLRPWM;
		/*PWM mode define*/
		PWM_IMDEPENDENT_MODE;
		/**********************************************************************
		PWM frequency = Fpwm/((PWMPH,PWMPL) + 1) <Fpwm = Fsys/PWM_CLOCK_DIV> 
									= (16MHz/4)/(0x9F + 1)
									= 25KHz
		***********************************************************************/
/*	
	PWMPH = 0x00;
		PWMPL = 0x9F;
	*/
		PWMPH = 0x00;
		PWMPL = 0xC7;
}
void PWM_duty(uint16_t duty)
{
	PWM3H = HIBYTE(duty);
  PWM3L = LOBYTE(duty);
  set_LOAD;
}

/*-------------------------DISPLAY-------------------------*/
void Display_cont_on(void)
{
	OLED_Clear();
	OLED_ShowString(25,2,"CONTINUOUS",16);
	OLED_ShowString(60,4,"ON",16);

}
void Display_cont_off(void)
{
	OLED_Clear();
	OLED_ShowString(25,2,"CONTINUOUS",16);
	OLED_ShowString(57,4,"OFF",16);

}
void num_float(uint8_t num)
{
	switch (num)
	{
		case 0:
		{
			OLED_DrawBMP(64,1,88,6,zero);
			break;
		}
		case 1:
		{
			OLED_DrawBMP(64,1,88,6,one);
			break;
		}
		case 2:
		{
			OLED_DrawBMP(64,1,88,6,two);
			break;
		}
		case 3:
		{
			OLED_DrawBMP(64,1,88,6,three);
			break;
		}
		case 4:
		{
			OLED_DrawBMP(64,1,88,6,four);
			break;
		}
		case 5:
		{
			OLED_DrawBMP(64,1,88,6,five);
			break;
		}
		case 6:
		{
			OLED_DrawBMP(64,1,88,6,six);
			break;
		}
		case 7:
		{
			OLED_DrawBMP(64,1,88,6,seven);
			break;
		}
		case 8:
		{
			OLED_DrawBMP(64,1,88,6,eight);
			break;
		}
		case 9:
		{
			OLED_DrawBMP(64,1,88,6,nine);
			break;
		}
	}
}
void num_donvi(uint8_t num)
{
	switch (num)
	{
		case 0:
		{
			OLED_DrawBMP(32,1,56,6,zero);
			break;
		}
		case 1:
		{
			OLED_DrawBMP(32,1,56,6,one);
			break;
		}
		case 2:
		{
			OLED_DrawBMP(32,1,56,6,two);
			break;
		}
		case 3:
		{
			OLED_DrawBMP(32,1,56,6,three);
			break;
		}
		case 4:
		{
			OLED_DrawBMP(32,1,56,6,four);
			break;
		}
		case 5:
		{
			OLED_DrawBMP(32,1,56,6,five);
			break;
		}
		case 6:
		{
			OLED_DrawBMP(32,1,56,6,six);
			break;
		}
		case 7:
		{
			OLED_DrawBMP(32,1,56,6,seven);
			break;
		}
		case 8:
		{
			OLED_DrawBMP(32,1,56,6,eight);
			break;
		}
		case 9:
		{
			OLED_DrawBMP(32,1,56,6,nine);
			break;
		}
	}
}
void num_chuc(uint8_t num)
{
	switch (num)
	{
		case 0:
		{
			OLED_DrawBMP(8,1,32,6,zero);
			break;
		}
		case 1:
		{
			OLED_DrawBMP(8,1,32,6,one);
			break;
		}
		case 2:
		{
			OLED_DrawBMP(8,1,32,6,two);
			break;
		}
		case 3:
		{
			OLED_DrawBMP(8,1,32,6,three);
			break;
		}
		case 4:
		{
			OLED_DrawBMP(8,1,32,6,four);
			break;
		}
		case 5:
		{
			OLED_DrawBMP(8,1,32,6,five);
			break;
		}
		case 6:
		{
			OLED_DrawBMP(8,1,32,6,six);
			break;
		}
		case 7:
		{
			OLED_DrawBMP(8,1,32,6,seven);
			break;
		}
		case 8:
		{
			OLED_DrawBMP(8,1,32,6,eight);
			break;
		}
		case 9:
		{
			OLED_DrawBMP(8,1,32,6,nine);
			break;
		}
	}
}
void Show_Voltage(double v_floatNum_f32)
{         	
	uint32_t v_temp_u32;
	uint8_t donvi;
	uint8_t chuc;
	v_temp_u32 = (uint32_t) v_floatNum_f32;
	chuc = v_temp_u32/10;
	donvi = v_temp_u32%10;
	if( chuc > 0)  num_chuc(chuc);
	else OLED_DrawBMP(8,1,32,6,clear);
	//OLED_Clear();
	num_donvi(donvi);
	v_floatNum_f32 = v_floatNum_f32 - v_temp_u32;
	v_temp_u32 = v_floatNum_f32 * 10;
	OLED_DrawBMP(56,5,64,6,dot);
	num_float(v_temp_u32);
} 
void displayProtect(void)
{
	OLED_DrawBMP(0,0,128,8,overcurrent);
	LED_PIN = 0;
	BUZZ_PIN = 1;
	Timer0_Delay1ms(500);
	BUZZ_PIN = 0;
	Timer0_Delay1ms(500);
	BUZZ_PIN = 1;
	Timer0_Delay1ms(500);
	BUZZ_PIN = 0;
	Timer0_Delay1ms(500);
	BUZZ_PIN = 1;
	Timer0_Delay1ms(500);
	BUZZ_PIN = 0;
	Timer0_Delay1ms(500);
	LED_PIN = 0;
	OLED_Clear();
}

unsigned int ADC_read(void){
	unsigned int adc_value = 0x0000;
	ADCCON1|=0X30;            /* clock divider */
  ADCCON2|=0X0E;            /* AQT time */
  AUXR1|=SET_BIT4;          /* ADC clock low speed */
  clr_ADCF;              
  set_ADCS;                   
  while(ADCF == 0);
  adc_value = ADCRH;
  adc_value <<= 4;
  adc_value |= ADCRL;
  return adc_value;
}


void SW_Reset(void)
{
    TA = 0xAA;
    TA = 0x55;
    set_SWRST;
}
void set_ADC_comparator_value(unsigned int value)
{
  ADCMPH = ((value & 0x0FF0) >> 4);
  ADCMPL = (value & 0x000F);
}