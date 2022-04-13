/* Decription:
		 - Padel Mode
		 - Continuous Mode
		 - Nitro Mode    
		 - Change Voltage
		 - Current          - 10A
		 - Sleep Mode       - 5s
		 - Auto Sleep mode  - 1p30s   
*/

#include "main.h"
#include "mode.h"
/*---------------------------------------------------------------------------------------------------------*/
/*Global Variable                                                                                              	 */
/*---------------------------------------------------------------------------------------------------------*/
double u16_count = 5.0;
uint16_t u16_duty = 50;
uint32_t u32_duty = 0;
uint8_t u8_contStatus = 0;
uint8_t u8_pedalStatus = 0;
uint8_t u8_contpedalStatus = 0;
unsigned int adc_value = 0x0000;
uint8_t u8_mode = 0,count =0, u8_sleep = 0,u8_switch = 0, u8_cntCur = 0,u8_nitroMode = 0,u8_nitroStart = 0;
uint32_t u32_countSec = 0,u32_countMin = 0;
/*---------------------------------------------------------------------------------------------------------*/
/* Main Function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
void main()
{	
		HW_init();
		u32_duty = 0;
		PWM_duty(0);
	  u16_count = 5.0;
		//InitialUART1_Timer3(115200);
		while(1)
		{
			set_ADCS;
			set_TR1;
			if(	adc_value > 350)
			{
				u8_cntCur = 0;
			}			
					/************ SET VOLTAGE ****************/
					if (ENC_PIN == 0) 
					{
						Timer0_Delay1ms(5);
						if( ENC_PIN == 0)
						{
							if ( u8_mode == 2 && u8_nitroStart == 0)
							{
								if(u8_nitroMode == 0)
								{
									OLED_Clear();
									u8_mode = 3;
									u8_nitroStart = 0;
								}
								else
								{
									OLED_Clear();
									u8_nitroStart = 1;
								}
							}
							else if (u8_mode == 0 || u8_mode == 1 || u8_nitroStart == 1)
							{
								u32_duty = (u16_count*10);
								//u16_duty = u32_duty*0.5;
								if(u8_mode == 1 && u8_contpedalStatus == 0 && u8_sleep == 0)
								{
									PWM_duty(u32_duty);
								}
								set_PWMRUN;
							}
								/************ SLEEP MODE ****************/
								while(ENC_PIN == 0)
								{
									count++;
									Timer0_Delay1ms(1000);
									if(count > 3 && u8_switch == 0) 
									{
										u8_switch = 1;
										count = 0;
										if(u8_sleep == 0) 
										{
											OLED_Display_Off();
											u8_sleep = 1;
											count = 0;
											u32_duty = 0;
											PWM_duty(0);
	//										SW_Reset();
											LED_PIN = 1;
											P11_Input_Mode;
											clr_EX0;
										}
										else 
										{
											OLED_Display_On();
											u8_sleep = 0;
											count = 0;
	//										u32_duty = 0;
	//										PWM_duty(0
											SW_Reset();
											LED_PIN = 1;
											P11_PushPull_Mode;
											set_EX0;
										}
									}
								}
						}
					}
					u8_switch = 0;
					count = 0;
					/************ CONTINUOUS MODE ****************/
						if (CONT_PIN == 0) 
						{
							Timer0_Delay1ms(5);
							if( CONT_PIN == 0)
							{
								if (u8_mode == 1) 
								{
									OLED_Clear();
									PWM_duty(0);
									LED_PIN = 1;
//									Display_cont_off();		
//									Timer0_Delay1ms(200);	
//									OLED_Clear();						
									u8_mode =2;	
								}
								else if (u8_mode == 0) 
								{
									OLED_Clear();
									u8_mode = 1;
									PWM_duty(u32_duty);
									LED_PIN = 0;
//									Display_cont_on();
//									Timer0_Delay1ms(200);
//									OLED_Clear();
								}
								else if (u8_mode  == 2)
								{
									OLED_Clear();
									u8_mode =3;
									u8_nitroMode = 0;
									u8_nitroStart = 0;
								}
								else if (u8_mode  == 3)
								{
									OLED_Clear();
									u8_mode =0;
								}
								while(CONT_PIN == 0);
							}
						}
					/************ MODES SYSTEM ****************/
					if (u8_mode == 0)
					{
						OLED_DrawBMP(88,1,128,4,pedal_lcd);
						/************ CONT PEDAL MODE ****************/
						if (PEDAL_PIN == 0) 
						{
								PWM_duty(u32_duty);
								LED_PIN = 0;
						}
						else 
						{
							PWM_duty(0);
							LED_PIN = 1;
						}	
					}
					else if ( u8_mode == 1)
					{
						OLED_DrawBMP(88,1,128,4,cont);
						/************ CONT PEDAL MODE ****************/
						if (PEDAL_PIN == 0) 
						{
							Timer0_Delay1ms(5);
							if( PEDAL_PIN == 0)
							{
								if (u8_contpedalStatus == 0) 
								{
									u8_contpedalStatus =1;
									PWM_duty(0);
									LED_PIN = 1;
								}
								else 
								{
									u8_contpedalStatus = 0;
									PWM_duty(u32_duty);
									LED_PIN = 0;
								}
								while(PEDAL_PIN == 0 );
							}
						}
					}
					else if ( u8_mode == 2)
					{
						OLED_DrawBMP(88,1,128,4,nitro);
						if(u8_nitroStart == 1)
						{
							if (PEDAL_PIN == 0) 
							{
								Timer0_Delay1ms(5);
								if( PEDAL_PIN == 0)
								{
									LED_PIN = 0;
									u32_duty = ((u16_count+2)*10);
									PWM_duty(u32_duty);
									Timer3_Delay100ms(5);
									u32_duty = ((u16_count)*10);
									PWM_duty(u32_duty);
									while(PEDAL_PIN == 0 );
								}
							}
		
							else 
							{
								LED_PIN = 1;
								PWM_duty(0);		
							}	
						}
						else
						{
							if (u8_nitroMode == 0)
							{
								OLED_DrawBMP(88,4,120,6,no_line);
								OLED_DrawBMP(88,6,120,8,yes);
							}
							else
							{
								OLED_DrawBMP(88,4,120,6,no);
								OLED_DrawBMP(88,6,120,8,yes_line);
							}
						}
					}
					else if ( u8_mode == 3)
					{
						OLED_DrawBMP(88,1,128,4,clean_mode);
					}
					Show_Voltage(u16_count);
					OLED_ShowString(8,6,"DUTY:",16);
					OLED_ShowString(70,6,"%",16);
					OLED_ShowNum(50,6,u16_duty,2,16);
				}		
}
/*---------------------------------------------------------------------------------------------------------*/
/* Interrupt - Voltage SETUP                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
void ADC_ISR(void)
interrupt 11	
{
  adc_value = ADC_read();	
	//LED_PIN = ~LED_PIN;
	
		if(adc_value > 3900)  // 3072 5A Overcurent
			{
				u32_duty = 0;
				PWM_duty(0);
				displayProtect();
				SW_Reset();
			}
  clr_ADCF;
}

void INT0_ISR(void) interrupt 0
{
	if ( u8_mode == 0 || u8_mode == 1 || u8_nitroStart == 1)
	{
		if (P17 == 1) 
		{
			BUZZ_PIN = 1;
			Timer1_Delay10ms(3);
			BUZZ_PIN = 0;
			u16_count+=0.1;
			if (u16_count > 17.9) u16_count = 18.0;
		}
		else 
		{
			BUZZ_PIN = 1;
			Timer1_Delay10ms(1);
			BUZZ_PIN = 0;
			u16_count-=0.1;
			if (u16_count < 2.2) u16_count = 2.1;
		}
	}
	if(u8_mode == 2 && u8_nitroStart == 0)
	{
		if (P17 == 1) 
		{
			BUZZ_PIN = 1;
			Timer1_Delay10ms(3);
			BUZZ_PIN = 0;
			u8_nitroMode = 1;
		}
		else 
		{
			BUZZ_PIN = 1;
			Timer1_Delay10ms(3);
			BUZZ_PIN = 0;
			u8_nitroMode = 0;
		}
	}
	if (u8_mode == 3)
	{
		if (P17 == 1) 
		{
			BUZZ_PIN = 1;
			Timer1_Delay10ms(3);
			BUZZ_PIN = 0;
			u16_duty+=1;
			if (u16_duty > 95) u16_duty = 95;
		}
		else 
		{
			BUZZ_PIN = 1;
			Timer1_Delay10ms(3);
			BUZZ_PIN = 0;
			u16_duty-=1;
			if (u16_duty < 0) u16_duty = 0;
		}
	}
	clr_TF0;
}
void TIM1_ISR(void) interrupt 3
{
		TL1 = LOBYTE(TIMER_DIV12_VALUE_1ms);	//Find  define in "Function_define.h" "TIMER VALUE"
    TH1 = HIBYTE(TIMER_DIV12_VALUE_1ms);
		/* Your code */
    //P13 =~ P13 ;				/* GPIO toggle when interrupt  */
		u32_countSec++;
		if(u32_countSec == 60000)
		{
			u32_countMin++;
			u32_countSec = 0;
		}
		if(u32_countMin == 5)
		{
	//	BUZZ_PIN =~ BUZZ_PIN;
			if(	adc_value < 350)
			{
				u8_cntCur++;
			}			

			if(u8_cntCur == 3)
			{
				OLED_Display_Off();
				u8_sleep = 1;
				count = 0;
				u32_duty = 0;
				PWM_duty(0);
				LED_PIN = 1;
				P11_Input_Mode;
				clr_EX0;
				u8_cntCur = 0;
			}
			u32_countMin = 0;
		}
		TF1 = 0 ;
}

