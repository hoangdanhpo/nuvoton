/*---------------------------------------------------------------------------------------------------------*/
/* Include File                                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#include "main.h"
/*---------------------------------------------------------------------------------------------------------*/
/* Variable                                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
uint16_t g_u16CurrentSpeed = 0;
double g_u16CurrentSpeedTemp = 0;
uint16_t u16_periodhall = 0;
uint16_t adc_value = 0;
uint16_t adc_value_vin  = 0x0000;
uint16_t adc_value_target_speed = 0x0000;
uint16_t adc_value_current = 0x0000;
uint16_t hall_value  = 0x0000;
uint16_t u16PWMDutyValue;
uint8_t i,dir = 0;
/*---------------------------------------------------------------------------------------------------------*/
/* Function                                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
void InitGPIO(void);
void PWM_Complementary_Init(void);  /*PWM Center type define & complementary mode*/
void PWM_duty(uint16_t value);
void FB_PWM_Init(void);  
void PWM_DEAD_TIME_VALUE(UINT16	DeadTimeData); 
void InitTimer0(void);
void InitTimer1(void);
void InitTimer2forCapture(void);
void ADC_targetspeed(void);       
void ADC_voltage(void);           
void ADC_current(void);             
void ChangeMotorPhaseClockwise(void);
void ChangeMotorPhaseCounterClockwise(void);
long map(long x, long in_min, long in_max, long out_min, long out_max);
void Check_WDT_Reset_Config(void);
void WDT_config(void);
/*---------------------------------------------------------------------------------------------------------*/
/* Main                                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
int main()
{
	//-------- Initial Peripherals--------------
	InitialUART0_Timer3(115200);
	InitGPIO();
	//------------Bootstrap-----------
	PWM_H_A = 0;PWM_H_B = 0;PWM_H_C = 0;
	for ( i=0;i<100;i++)
	{
		PWM_L_A = 1; PWM_L_B = 1; PWM_L_C = 1; Timer0_Delay1ms(10);
		PWM_L_A = 0; PWM_L_B = 0; PWM_L_C = 0; Timer0_Delay1ms(10);
	}
	//--------Init PWM--------------
	PWM_Complementary_Init();
	FB_PWM_Init();
	
	PWM_DEAD_TIME_VALUE(32);
	PWM01_DEADTIME_ENABLE;
	PWM45_DEADTIME_ENABLE;
	PWM34_DEADTIME_ENABLE;
	//PWM_duty(50);
	
	//--------Init TIMER--------------
	InitTimer0();
	InitTimer1();
	InitTimer2forCapture();
	
	//--------Init WDT--------------
//	Check_WDT_Reset_Config();
//	WDT_config();
//	set_WDTR;	
//	set_WDCLR;

	//--------ADC start run--------------
	set_EADC;
	set_ADCS;                 // Start ADC 
  //-------- PWM start run--------------
  set_EPWM;									//Enable PWM interrupt
	set_EFB;
	set_EA;									
  set_LOAD;
  //set_PWMRUN;
	
	while(1)
	{		
//		if (DIR_PIN == 0) 
//		{
//			Timer0_Delay1ms(5);
//			if( DIR_PIN == 0)
//			{
//				if (dir == 0) dir = 1;
//				else dir = 0;
//				while(DIR_PIN == 0);
//			}
//		}
		
		ADC_targetspeed();
		// Channel 0 - P17
		u16PWMDutyValue = map(adc_value_target_speed, 0, 4095, 0, 95);
		if(u16PWMDutyValue>10)
		{
			set_PWMRUN;
			PWM_duty(u16PWMDutyValue);
			set_PWMRUN;
		}
		else 
		{
			u16PWMDutyValue = 0;
			PWM_duty(u16PWMDutyValue);
			clr_PWMRUN;
		}
			
//		printf ("\n Target Speed Value = %d",u16PWMDutyValue);  // Read u16PWMDutyValue
//		printf ("\n Target Speed Value = %d",adc_value_target_speed);
//		ADC_voltage();                                          // Channel 4 - P04  
//		printf ("\n Voltage value = %d",adc_value_vin);                

//		ADC_current();                                          // Channel 5 - P06
//		printf ("\n Current value = %d",adc_value_current);
		
//		Timer0_Delay1ms(200);

		//set_WDCLR;		
	}
}

/*****************************************CONFIGUARE*******************************************************/

/*---------------------------------------------------------------------------------------------------------*/
/* Init GPIO                                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
void InitGPIO(void)
{
		/* Use Uart0 TX: P06 RX: P07*/
		/* Hall sensor */
		P30_Input_Mode;  // Input only mode
		P16_Input_Mode;  // Input only mode
		P15_Input_Mode;  // Input only mode
	
    /* PWM */
    P12_PushPull_Mode;        // HA
    P11_PushPull_Mode;				// LA
    P10_PushPull_Mode;				// HB
    P00_PushPull_Mode;				// LB
    P01_PushPull_Mode;				// HC
    P03_PushPull_Mode;				// LC

	  /* Fault Brake Pin - OCP/DIS Function */
		P14_Input_Mode;   

		/* Pulse Signal Output */
		P13_PushPull_Mode;
		
}
/*---------------------------------------------------------------------------------------------------------*/
/* Init PWM                                                                                              	 */
/*---------------------------------------------------------------------------------------------------------*/
void PWM_Complementary_Init(void)
{
	PWM0_P12_OUTPUT_ENABLE;
	PWM1_P11_OUTPUT_ENABLE;
	PWM2_P10_OUTPUT_ENABLE;
	PWM3_P00_OUTPUT_ENABLE;
	PWM4_P01_OUTPUT_ENABLE;
	PWM5_P03_OUTPUT_ENABLE;
	/*PMW clock source select define*/
	PWM_GP_MODE_ENABLE;
	PWM_CLOCK_DIV_4;
	/*PWM type define*/
	PWM_CENTER_TYPE;
	/*PWM mode define*/
	PWM_COMPLEMENTARY_MODE;
	/* PWM interrupt pin select */
	PWM_INT_PWM0;
	/* PMW interrupt setting */
	PWM_PERIOD_END_INT;
	/*clear PWM 12bit counter*/
	set_CLRPWM;
	/**********************************************************************
	PWM frequency = Fpwm/((PWMPH,PWMPL)*2) < Fpwm = Fsys/PWM_CLOCK_DIV> 
								= (16MHz/8)/(0x3E8 * 2)
								= 1KHz (1ms)
	***********************************************************************/
		PWMPH = 0x00;
		PWMPL = 0x64;
	/* Initialize the pwm pin mode and close whole MOS. */
	  PMEN = 0xff;
    PMD = 0x00;
}
void PWM_duty(uint16_t value)
{
	PWM0H = HIBYTE(value);
  PWM0L = LOBYTE(value);
  set_LOAD;
}
void FB_PWM_Init(void)
{
	set_FBINEN;    //FB Pin Input Enable
	clr_FBINLS;    //Rising edge
}
/*-----------------------------------------------------------------------------------------------------------------
	Dead time 			= 0x20 <PDTEN.4+PDTCNT[7:0]>/Fsys = 0x20/Fsys =32/16000000 = 2 us (max value)
-----------------------------------------------------------------------------------------------------------------*/
void PWM_DEAD_TIME_VALUE(UINT16	DeadTimeData)
{
	UINT8 deadtmphigh,deadtmplow;
	deadtmplow = DeadTimeData;
	deadtmphigh = DeadTimeData>>8;
	BIT_TMP = EA;
	if (deadtmphigh==0x01)
	{
		EA = 0;
		TA = 0xAA;
		TA = 0x55;
		PDTEN|=0x10;
	}
	TA = 0xAA;
	TA = 0x55;
	PDTCNT = deadtmplow;
	EA = BIT_TMP;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Init timer                                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
void InitTimer0(void)
{
    clr_T0M;        /* T0M=0, Timer0 Clock = Fsys/12 */
    TMOD |= 0x01;   /* Timer0 is 16-bit mode */
    
    /* Calculate the timer counter value for controlling the interrupt period on 10ms. */
    TL0 = LOBYTE(TIMER_DIV12_VALUE_10ms);	//Find  define in "Function_define.h" "TIMER VALUE"
    TH0 = HIBYTE(TIMER_DIV12_VALUE_10ms);

    set_ET0;        /* enable Timer0 interrupt */
		set_EA;
    set_TR0;        /* Timer0 start */
}
void InitTimer1(void)
{
    clr_T1M;        /* T1M=0, Timer0 Clock = Fsys/12 */
    TMOD |= (1 << 4);
		TMOD &= ~(1 << 5);
    
    /* Calculate the timer counter value for controlling the interrupt period on 10ms. */
    TL1 = LOBYTE(TIMER_DIV12_VALUE_1ms);	//Find  define in "Function_define.h" "TIMER VALUE"
    TH1 = HIBYTE(TIMER_DIV12_VALUE_1ms);

    set_ET1;        /* enable Timer0 interrupt */
		set_EA;
    set_TR1;        /* Timer0 start */
}
void InitTimer2forCapture(void)
{
    /* Initial the Timer2 for capture motor speed */
    TIMER2_CAP0_Capture_Mode;
    IC7_P15_CAP0_RisingEdge_Capture;;
    TIMER2_DIV_512;
    /* Enable Capture interrupt */
    set_ECAP;
    /* Triger Timer2 */
    set_TR2;
}
/*---------------------------------------------------------------------------------------------------------*/
/* Init ADC                                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
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
void ADC_targetspeed(void)
{ 
	Enable_ADC_AIN0;
	clr_ADCF;              
  set_ADCS;
	while(ADCF == 0);
	adc_value_target_speed = ADCRH;
  adc_value_target_speed <<= 4;
  adc_value_target_speed |= ADCRL;		
	Disable_ADC;
	//printf ("\n Target Speed Value = %d",adc_value_target_speed);  
}

void ADC_voltage(void)
{ 
	Enable_ADC_AIN4;
	clr_ADCF;              
  set_ADCS;
	while(ADCF == 0);
	ADCMPL = 0xC;
	ADCMPH = 0x5D;            // Threshold = 1500
	ADCCON2 |= 0xA0; 					// ADC Result Comparator Enable & ADC Compare Result Asserting Fault Brake Enable
	adc_value = ADCRH;
  adc_value <<= 4;
  adc_value |= ADCRL;		
	ADCCON2 =0;
	adc_value_vin = adc_value;
	Disable_ADC;
	//printf ("\n Voltage value = %d",adc_value_vin);
}

void ADC_current(void)
{
	Enable_ADC_AIN5;
	adc_value = ADCRH;
  adc_value <<= 4;
  adc_value |= ADCRL;		
  adc_value_current = adc_value;
	clr_ADCF;              
  set_ADCS;
}

void ChangeMotorPhaseClockwise(void)
{
		hall_value = P15;
		hall_value<<=1;
		hall_value |= P16;
		hall_value<<=1;
		hall_value |= P30;
	
    /* Change motor phase to next phase. */
		//printf ("\n hall value = %d",hall_value);
    switch (hall_value)
    {
        case 6:
        {
            PMEN = 0xfc;
						PMD = 0x20;
            break;
        }
        case 4:
        {
            PMEN = 0xf3;
						PMD = 0x20;
            break;
        }
        case 5:
        {
            PMEN = 0xf3;
						PMD = 0x02;
            break;
        }
        case 1:
        {
            PMEN = 0xcf;
						PMD = 0x02;
            break;
        }
        case 3: 
        {
            PMEN = 0xcf;
						PMD = 0x08;
            break;
        }
        case 2:
        {
            PMEN = 0xfc;
						PMD = 0x08;
            break;
        }
    }
}


void ChangeMotorPhaseCounterClockwise(void)
{
	  hall_value = P15;
		hall_value<<=1;
		hall_value |= P16;
		hall_value<<=1;
		hall_value |= P30;
	
    /* Change motor phase to next phase. */
		//printf ("\n hall value = %d",hall_value);
    switch (hall_value)
    {
         case 1:
        {
            PMEN = 0xfc;
						PMD = 0x20;
            break;
        }
        case 3:
        {
            PMEN = 0xf3;
						PMD = 0x20;
            break;
        }
        case 2:
        {
            PMEN = 0xf3;
						PMD = 0x02;
            break;
        }
        case 6:
        {
            PMEN = 0xcf;
						PMD = 0x02;
            break;
        }
        case 4: 
        {
            PMEN = 0xcf;
						PMD = 0x08;
            break;
        }
        case 5:
        {
            PMEN = 0xfc;
						PMD = 0x08;
            break;
        }
    }
}
/*---------------------------------------------------------------------------------------------------------*/
/* Interrupt                                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
void Timer0_ISR (void) interrupt 1           /*interrupt address is 0x000B */
{
    TL0 = LOBYTE(TIMER_DIV12_VALUE_10ms);	//Find  define in "Function_define.h" "TIMER VALUE"
    TH0 = HIBYTE(TIMER_DIV12_VALUE_10ms);
		/* Your code */
    //P14 = ~P14;	/* GPIO toggle when interrupt  */
		//P13 =~ P13 ;				/* GPIO toggle when interrupt  */
		TF0 = 0 ;
}
void TIM1_ISR(void) interrupt 3
{
		TL1 = LOBYTE(TIMER_DIV12_VALUE_1ms);	//Find  define in "Function_define.h" "TIMER VALUE"
    TH1 = HIBYTE(TIMER_DIV12_VALUE_1ms);
		/* Your code */
    //P13 =~ P13 ;				/* GPIO toggle when interrupt  */
	
		TF1 = 0 ;
}

void Capture_ISR(void) interrupt 12
{
//    /* Clear capture0 interrupt flag */
//    clr_CAPF0;
//		u16_periodhall = (C0L + (C0H << 8))*(1/31250);
//    /* Get the current motor speed */
//    g_u16CurrentSpeed = 60/u16_periodhall ;

//    clr_TF2;
	  /* Clear capture0 interrupt flag */
    clr_CAPF0;
    /* Get the current motor speed */
    g_u16CurrentSpeedTemp = (C0L + (C0H << 8))*0.000032;
    g_u16CurrentSpeed = 60/(g_u16CurrentSpeedTemp*2);
	  C0L = 0; C0H = 0;
    clr_TF2;
}
void FB_ISR(void) interrupt 14
{ 
  clr_FBF;
	
}

void PWM_ISR (void) interrupt 13
{

    clr_PWMF;               // clear PWM interrupt flag
  //  ADC_current();    

	ChangeMotorPhaseClockwise(); 	// Channel 5 - P06
	//ChangeMotorPhaseCounterClockwise();
//	if (dir == 0) ChangeMotorPhaseClockwise();
//	else ChangeMotorPhaseCounterClockwise();
}
long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void Check_WDT_Reset_Config(void)
{
		set_IAPEN;
		IAPAL = 0x04;
		IAPAH = 0x00;	
		IAPFD = 0xFF;
		IAPCN = 0xC0;						                       
		set_IAPGO;  
		if ((IAPFD&0xF0)==0xF0)
		{
				IAPFD = 0x0F;
				IAPCN = 0xE1;
        set_IAPGO;                         						
				while((CHPCON&SET_BIT6)==SET_BIT6);          
				clr_CFUEN;
				clr_IAPEN;
				EA = 0;
				TA = 0xAA;
				TA = 0x55;
				CHPCON &= 0xFD;
				TA = 0xAA;
				TA = 0x55;
				CHPCON |= 0x80;
		}
		clr_IAPEN;
}

void WDT_config(void)
{
	TA = 0xAA;
	TA = 0x55;
	WDCON |= 0x07;        // config driver = 1/256 => time out = 1.638s
	set_WDCLR;            // clr WDT counter 0
}

