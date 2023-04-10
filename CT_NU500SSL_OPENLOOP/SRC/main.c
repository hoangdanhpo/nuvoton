/*---------------------------------------------------------------------------------------------------------*/
/* Include File                                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#include "main.h"
/*---------------------------------------------------------------------------------------------------------*/
/* Macro                                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
#define NORMAL_TEST
/*---------------------------------------------------------------------------------------------------------*/
/* Variable                                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
uint16_t g_u16CurrentSpeed = 0;
double g_u16CurrentSpeedTemp = 0;
uint16_t u16_periodhall = 0;
uint16_t hall_value  = 0x0000;
uint16_t u16PWMDutyValue;
uint16_t u16AdcValue;
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
void ChangeMotorPhaseClockwise(void);
void ChangeMotorPhaseCounterClockwise(void);
long map(long x, long in_min, long in_max, long out_min, long out_max);
void Check_WDT_Reset_Config(void);
void WDT_config(void);

void ADC_Init(void);
void ADC_SelectChannel(uint8_t u8Channel);
void ADC_StartConv(void);
uint8_t ADC_IsBusy(void);
uint16_t ADC_GetData(void);
void ADC_ClearFlag(void);
void ADC_Enable(void);
void ADC_Disable(void);
void ADC_ISR(void);
uint8_t Get_Channel_ADC(void);
void ADC_ClearRegRes(void);
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
    for ( i = 0; i < 100; i++ )
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

    //--------Init TIMER--------------
    InitTimer0();
    InitTimer1();
    InitTimer2forCapture();

    //--------Init WDT--------------
#ifdef ENABLE_WDG
    Check_WDT_Reset_Config();
    WDT_config();
    set_WDTR;
    set_WDCLR;
#endif

    //--------ADC start run--------------
    ADC_Init();

    //-------- PWM start run--------------
    /* Enable PWM interrupt */
    set_EPWM;
    set_EFB;
    set_EA;
    set_LOAD;

#ifdef NORMAL_TEST
    PWM_duty(50);
    set_PWMRUN;
#endif

    while(1)
    {
#ifdef SPEED_TEST

    ADC_SelectChannel(0);
    ADC_StartConv();
    u16PWMDutyValue = map(u16AdcValue, 0, 4095, 0, 95);
    printf("Duty value: %d \n\r", u16PWMDutyValue);
    ADC_ClearRegRes();
    if(u16PWMDutyValue > 10)
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

#endif

#ifdef MEASURE_VOLTAGE_CUR_TEST

    ADC_SelectChannel(4);
    ADC_StartConv();
    printf("Voltage value: %d\n\r", u16AdcValue);
    ADC_ClearRegRes();

    ADC_SelectChannel(5);
    ADC_StartConv();
    printf("Current value: %d\n\r", u16AdcValue);
    ADC_ClearRegRes();

    Timer0_Delay1ms(100);

#endif

#ifdef ENABLE_WDG
    set_WDCLR;
#endif

    }
}

/*****************************************CONFIGUARE*******************************************************/

/*---------------------------------------------------------------------------------------------------------*/
/* Init GPIO                                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
void InitGPIO(void)
{
    /* Hall sensor */
    P30_Input_Mode;             /* Input only mode */
    P16_Input_Mode;             /* Input only mode */
    P15_Input_Mode;             /* Input only mode */

    /* PWM */
    P12_PushPull_Mode;          // HA
    P11_PushPull_Mode;          // LA
    P10_PushPull_Mode;          // HB
    P00_PushPull_Mode;          // LB
    P01_PushPull_Mode;          // HC
    P03_PushPull_Mode;          // LC

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
    /* FB Pin Input Enable */
    set_FBINEN;

    /* Rising edge */
    clr_FBINLS;
}

/*-----------------------------------------------------------------------------------------------------------------
    Dead time = 0x20 <PDTEN.4+PDTCNT[7:0]>/Fsys = 0x20/Fsys =32/16000000 = 2 us (max value)
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
    TL0 = LOBYTE(TIMER_DIV12_VALUE_10ms);   //Find  define in "Function_define.h" "TIMER VALUE"
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
/* Control Motor                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
void ChangeMotorPhaseClockwise(void)
{
    hall_value = P15;
    hall_value<<=1;
    hall_value |= P16;
    hall_value<<=1;
    hall_value |= P30;

    /* Change motor phase to next phase. */
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

    /* Testing */
    //P14 = ~P14;	/* GPIO toggle when interrupt  */
    //P13 =~ P13 ;  /* GPIO toggle when interrupt  */

    TF0 = 0 ;
}

void TIM1_ISR(void) interrupt 3
{
    TL1 = LOBYTE(TIMER_DIV12_VALUE_1ms);	//Find  define in "Function_define.h" "TIMER VALUE"
    TH1 = HIBYTE(TIMER_DIV12_VALUE_1ms);

    /* Testing */
    //P13 =~ P13 ; /* GPIO toggle when interrupt  */

    TF1 = 0 ;
}

void Capture_ISR(void) interrupt 12
{
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
    //ADC_current();

    ChangeMotorPhaseClockwise(); 	// Channel 5 - P06
    //ChangeMotorPhaseCounterClockwise();

}

long map(long x, long in_min, long in_max, long out_min, long out_max)
{
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

/*---------------------------------------------------------------------------------------------------------*/
/* Init ADC                                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
void ADC_Init(void)
{
    ADCCON1 |= (1 << 0);
    /* Clock */
    ADCCON1 &= ~(0x30);
    ADCCON1 |= 0x00;

    /* Channel 0 */
    P1M1 |= (1 << 7);
    P1M2 &= ~(1 << 7);
    AINDIDS |= (1 << 0);

    /* Channel 4 */
    P0M1 |= (1 << 5);
    P0M2 &= ~(1 << 5);
    AINDIDS |= (1 << 4);

    /* Channel 5 */
    P0M1 |= (1 << 4);
    P0M2 &= ~(1 << 4);
    AINDIDS |= (1 << 5);

    EADC = 1;
}

void ADC_SelectChannel(uint8_t u8Channel)
{
    ADCCON0 &= ~(0x0F);
    ADCCON0 |= (u8Channel & 0x07);
}

void ADC_StartConv(void)
{
    ADCS = 1;
}

uint8_t ADC_IsBusy(void)
{
    return ADCS;
}

uint16_t ADC_GetData(void)
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

void ADC_ISR(void) interrupt 11
{
    /* Get data ADC */
    u16AdcValue = ADC_GetData();

    ADC_ClearFlag();
    ADC_StartConv();
}

uint8_t Get_Channel_ADC(void)
{
    unsigned char lower_4_bits = ADCCON0 & 0x0F; // Get lower 4 bits of register
    return lower_4_bits;
}

void ADC_ClearRegRes(void)
{
    ADCRH = 0x00;
    ADCRL = 0x00;
}
