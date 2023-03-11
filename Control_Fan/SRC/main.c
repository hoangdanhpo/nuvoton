/*---------------------------------------------------------------------------------------------------------*/
/* Include File                                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
/*---------------------------------------------------------------------------------------------------------*/
/* Macro			                                                                                             */
/*---------------------------------------------------------------------------------------------------------*/
#define DEFAULT_DELAY 	1000
#define DEBOUNCE_FAN 		250
/*---------------------------------------------------------------------------------------------------------*/
/* Variable                                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
uint16_t u16DelayRelay;
uint16_t u16AdcValue;

uint8_t u8StatusPinFAN;
uint8_t u8StatusPinKEYCARD;
uint8_t u8StatusPinWINDOWN;

uint8_t F_Case;
uint8_t Is_F_Case_1;
uint8_t Is_F_Case_2;
uint8_t Is_F_Case_3;
uint16_t u16Count;
/*---------------------------------------------------------------------------------------------------------*/
/* Function                                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
void InitGPIO(void);                
void Active_Relay(uint16_t delay);
void Default_Mode(void);
void Update_Status_PIN(void);
long map(long x, long in_min, long in_max, long out_min, long out_max);
/*---------------------------------------------------------------------------------------------------------*/
/* Main                                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
int main()
{
	//-------- Initial Peripherals--------------
	InitGPIO();
	RELAY_PIN = 0;
	F_Case = 0;
	Is_F_Case_1 = OFF;
	Is_F_Case_2 = OFF;
	Is_F_Case_3 = OFF;
	//--------ADC start run--------------
	ADC_Init();
	ADC_SelectChannel(4);
	ADC_StartConv();
	set_EA;
	
	while(1)
	{		
		Update_Status_PIN();
		
		/* Default case */
		if ( ( OFF == u8StatusPinFAN )
			&& (NOT_INSERTED == u8StatusPinKEYCARD)
			&& (CLOSED == u8StatusPinWINDOWN) ) 
		{
			Default_Mode();
		}
		
		if ( ( ON == u8StatusPinFAN )
			&& (INSERTED == u8StatusPinKEYCARD)
			&& (CLOSED == u8StatusPinWINDOWN) ) 
		{
			Default_Mode();
		}
		
		if ( ( OFF == u8StatusPinFAN )
			&& (NOT_INSERTED == u8StatusPinKEYCARD)
			&& (CLOSED == u8StatusPinWINDOWN) ) 
		{
			Default_Mode();
		}
		
		if ( ( OFF == u8StatusPinFAN )
			&& (INSERTED == u8StatusPinKEYCARD)
			&& (NOT_CLOSED == u8StatusPinWINDOWN) ) 
		{
			Default_Mode();
		}
		
		if ( ( ON == u8StatusPinFAN )
			&& (NOT_INSERTED == u8StatusPinKEYCARD)
			&& (NOT_CLOSED == u8StatusPinWINDOWN) ) 
		{
			Default_Mode();
		}
		
		/* Active case */
		
		if ( ( OFF == u8StatusPinFAN )
			&& (INSERTED == u8StatusPinKEYCARD)
			&& (CLOSED == u8StatusPinWINDOWN) 
		  	&& (Is_F_Case_1 == OFF)) 
		{
			Is_F_Case_1 = ON;
			F_Case = 1;
		}
		
		else if ( ON == u8StatusPinFAN )
		{
			if ( ( NOT_INSERTED == u8StatusPinKEYCARD ) 
				&& (CLOSED == u8StatusPinWINDOWN) 
				&& (Is_F_Case_2 == OFF) )
			{
				Is_F_Case_2 = ON;
				F_Case = 2;
			}
			if ( ( INSERTED == u8StatusPinKEYCARD ) 
				&& (NOT_CLOSED == u8StatusPinWINDOWN) 
				&& (Is_F_Case_3 == OFF) )
			{
				Is_F_Case_3 = ON;
				F_Case = 3;
			}
		}
		else
		{
			Default_Mode();
		}
		
		switch(F_Case)
		{
			case 1:
			{
				Update_Status_PIN();
				
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
				}
				
				u16Count = 0;
				while ( (INSERTED == u8StatusPinKEYCARD) && (CLOSED == u8StatusPinWINDOWN) )
				{
					Update_Status_PIN();
					RELAY_PIN = 0;
				}
				RELAY_PIN = 0;
				F_Case = 0;
				break;
			}

			case 2: 
			{
				Update_Status_PIN();
				
				if ( ON == u8StatusPinFAN )
				{
					Active_Relay(u16DelayRelay);
				}
				F_Case = 0;
				break;
			}
			case 3: 
			{
				Update_Status_PIN();
				
				if ( ON == u8StatusPinFAN )
				{
					Active_Relay(u16DelayRelay);
				}
				F_Case = 0;
				break;
			}
			
			default:
			{
				F_Case = 0;
				break;
			}
		}	
	}
}

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

void Default_Mode(void)
{
	F_Case = 0;
	Is_F_Case_1 = OFF;
	Is_F_Case_2 = OFF;
	Is_F_Case_3 = OFF;
	RELAY_PIN = 0;
}

void Update_Status_PIN(void)
{
	u8StatusPinFAN = FAN_PIN;
	u8StatusPinKEYCARD = KEYCARD_PIN;
	u8StatusPinWINDOWN = WINDOWN_SW_PIN;
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
