/*---------------------------------------------------------------------------------------------------------*/
/* Include File                                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
/*---------------------------------------------------------------------------------------------------------*/
/* Macro			                                                                                             */
/*---------------------------------------------------------------------------------------------------------*/
#define DEFAULT_DELAY 1000
#define DEBOUNCE_FAN 	250
#define TEST_RELAY
/*---------------------------------------------------------------------------------------------------------*/
/* Variable                                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
uint16_t u16DelayRelay;
uint16_t u16AdcValue;

uint8_t u8StatusPinFAN;
uint8_t u8StatusPinKEYCARD;
uint8_t u8StatusPinWINDOWN;

uint8_t F_Case = 0;
uint8_t Is_F_Case_2 = OFF;
uint8_t Is_F_Case_3 = OFF;
int cnt;
/*---------------------------------------------------------------------------------------------------------*/
/* Function                                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
void InitGPIO(void);                
void Active_Relay(uint16_t delay);
long map(long x, long in_min, long in_max, long out_min, long out_max);
/*---------------------------------------------------------------------------------------------------------*/
/* Main                                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
int main()
{
	//-------- Initial Peripherals--------------
	InitGPIO();
	RELAY_PIN = 0;
	//--------ADC start run--------------
	ADC_Init();
	ADC_SelectChannel(4);
	ADC_StartConv();
	set_EA;
	
	while(1)
	{		
		u8StatusPinFAN = FAN_PIN;	
		u8StatusPinKEYCARD = KEYCARD_PIN;	
		u8StatusPinWINDOWN = WINDOWN_SW_PIN;
		
		if ( ( OFF == u8StatusPinFAN )
			&& (INSERTED == u8StatusPinKEYCARD)
			&& (CLOSED == u8StatusPinWINDOWN) 
		  && (F_Case == 0)) 
		{
				F_Case = 1;
		}
		else if ( ON == u8StatusPinFAN )
		{
			if ( ( NOT_INSERTED == u8StatusPinKEYCARD ) && (CLOSED == u8StatusPinWINDOWN) && (Is_F_Case_2 == OFF) )
			{
					Is_F_Case_2 = ON;
					F_Case = 2;
			}
			if ( ( INSERTED == u8StatusPinKEYCARD ) && (NOT_CLOSED == u8StatusPinWINDOWN) && (Is_F_Case_3 == OFF) )
			{
					Is_F_Case_3 = ON;
					F_Case = 3;
			}
		}
		else
		{
			F_Case = 0;
			Is_F_Case_2 = OFF;
			Is_F_Case_3 = OFF;
			RELAY_PIN = 0;
		}
		
		if ( ( ON == u8StatusPinFAN )
				&& (INSERTED == u8StatusPinKEYCARD)
				&& (CLOSED == u8StatusPinWINDOWN) ) 
		{
				RELAY_PIN = 0;
				F_Case = 0;
		}
		
		switch(F_Case)
		{
			case 1:
			{
				u8StatusPinFAN = FAN_PIN;
				u8StatusPinKEYCARD = KEYCARD_PIN;
				u8StatusPinWINDOWN = WINDOWN_SW_PIN;
				
				if (FAN_PIN == ON) 
				{
					Timer0_Delay1ms(DEBOUNCE_FAN);
					if( FAN_PIN == ON)
					{
						u8StatusPinFAN = FAN_PIN;
					}
				}
				
				if ( OFF == u8StatusPinFAN )
				{
					Active_Relay(u16DelayRelay);
					Timer0_Delay1ms(DEFAULT_DELAY);
				}
				
				cnt = 0;
				while ( ( OFF == u8StatusPinFAN )
						&& (INSERTED == u8StatusPinKEYCARD)
						&& (CLOSED == u8StatusPinWINDOWN) )
				{
					u8StatusPinFAN = FAN_PIN;
					u8StatusPinKEYCARD = KEYCARD_PIN;
					u8StatusPinWINDOWN = WINDOWN_SW_PIN;
					
					cnt++;
					
					/* Debounce Fan Button */
					if (FAN_PIN == ON) 
					{
						Timer0_Delay1ms(DEBOUNCE_FAN);
						if( FAN_PIN == ON)
						{
							u8StatusPinFAN = FAN_PIN;
						}
					}
					
					if ( ON == u8StatusPinFAN )
					{
						RELAY_PIN = 0;
						break;
					}
					else 
					{
						if(cnt > 0 && cnt < 500)
						{
							RELAY_PIN = 1;
						}
						
						if(cnt > 500 && cnt < 2000)
						{
							RELAY_PIN = 0;
						}
						if ( cnt > 2000)
						{
							cnt = 0;
						}
					}
					
				}
				
				RELAY_PIN = 0;
				F_Case = 0;
				
				break;
			}
			case 2: 
			{
				u8StatusPinFAN = FAN_PIN;
				u8StatusPinKEYCARD = KEYCARD_PIN;
				u8StatusPinWINDOWN = WINDOWN_SW_PIN;
				
				if ( ON == u8StatusPinFAN )
					{
						Active_Relay(u16DelayRelay);
						Timer0_Delay1ms(DEFAULT_DELAY);
					}
					
				cnt = 0;
					
				while ( ( ON == u8StatusPinFAN )
						&& (NOT_INSERTED == u8StatusPinKEYCARD)
						&& (CLOSED == u8StatusPinWINDOWN) )
				{
					u8StatusPinFAN = FAN_PIN;
					u8StatusPinKEYCARD = KEYCARD_PIN;
					u8StatusPinWINDOWN = WINDOWN_SW_PIN;
					cnt++;
					
					if ( OFF == u8StatusPinFAN )
					{
						RELAY_PIN = 0;
						F_Case = 0;
						break;
					}
					else 
					{
						if(cnt > 0 && cnt < 500)
						{
							RELAY_PIN = 1;
						}
						
						if(cnt > 500 && cnt < 2000)
						{
							RELAY_PIN = 0;
						}
						if ( cnt > 2000)
						{
							cnt = 0;
						}
					}
				}
				Is_F_Case_2 = OFF;

				break;
			}
			case 3: 
			{
				u8StatusPinFAN = FAN_PIN;
				u8StatusPinKEYCARD = KEYCARD_PIN;
				u8StatusPinWINDOWN = WINDOWN_SW_PIN;
				
				if ( ON == u8StatusPinFAN )
					{
						Active_Relay(u16DelayRelay);
						Timer0_Delay1ms(DEFAULT_DELAY);
					}
				cnt = 0;
					
				while ( ( ON == u8StatusPinFAN )
						&& (INSERTED == u8StatusPinKEYCARD)
						&& (NOT_CLOSED == u8StatusPinWINDOWN) )
				{
					u8StatusPinFAN = FAN_PIN;
					u8StatusPinWINDOWN = WINDOWN_SW_PIN;
					u8StatusPinKEYCARD = KEYCARD_PIN;
					
					cnt++;
					
					if ( OFF == u8StatusPinFAN )
					{
						RELAY_PIN = 0;
						F_Case = 0;
						break;
					}
					else 
					{
						if(cnt > 0 && cnt < 500)
						{
							RELAY_PIN = 1;
						}
						
						if(cnt > 500 && cnt < 2000)
						{
							RELAY_PIN = 0;
						}
						if ( cnt > 2000)
						{
							cnt = 0;
						}
					}
				}
				Is_F_Case_3 = OFF;
				break;
			}
			
			default:
			{
				F_Case = 0;
				/* Do nothing */
				break;
			}
		}	
	}
}

/*****************************************CONFIGUARE*******************************************************/
/*---------------------------------------------------------------------------------------------------------*/
/* Init GPIO                                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
void InitGPIO(void)
{
		/* Keycard */
		P11_Input_Mode;  		
		/* Window switch */
		P00_Input_Mode;  		
		/* FAN */
		P00_Input_Mode; 	
		/* RELAY */
		P03_PushPull_Mode; 
}

void Active_Relay(uint16_t delay)
{
	RELAY_PIN = 1;
	Timer0_Delay1ms(delay);
	RELAY_PIN = 0;
}

void ADC_ISR(void) interrupt 11
{
	
	u16AdcValue = ADC_GetData();
	u16DelayRelay = map(u16AdcValue , 0, 1248, 100 , 5000); 
	ADC_ClearFlag();
	ADC_StartConv();
}

long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
