/*---------------------------------------------------------------------------------------------------------*/
/* Include File                                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#include "main.h"
/*---------------------------------------------------------------------------------------------------------*/
/* Macro                                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
#define HOLD_TIME           2000
#define NUM_SPEED_DEFAULT   0
#define NUM_SPEED_L         1
#define NUM_SPEED_M         2
#define NUM_SPEED_H         3
#define NUM_SPEED_MAX       4
/*---------------------------------------------------------------------------------------------------------*/
/* Variable                                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t IsActiveRelay;
uint8_t StatusSpeed1;
uint8_t StatusSpeed2;
uint8_t StatusSpeed3;
uint8_t NumControlFan;
uint8_t temp = 0;
/*---------------------------------------------------------------------------------------------------------*/
/* Function                                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
void InitGPIO(void);
void Set_RL_FAN_Default(void);
void Set_RL_Speed_1(void);
void Set_RL_Speed_2(void);
void Set_RL_Speed_3(void);
/*---------------------------------------------------------------------------------------------------------*/
/* Typedef                                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
typedef void (*t_Speed_FUNC)( void );
const t_Speed_FUNC arr_Speed_Func[NUM_SPEED_MAX] =
{
    &Set_RL_FAN_Default,
    &Set_RL_Speed_1,
    &Set_RL_Speed_2,
    &Set_RL_Speed_3
};
/*---------------------------------------------------------------------------------------------------------*/
/* Main                                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
int main()
{
    /*-------- Initial Peripherals--------------*/
    InitGPIO();
    IsActiveRelay = OFF;
    NumControlFan = NUM_SPEED_DEFAULT;
    while(1)
    { 
			#if 1
        if ( IsActiveRelay == OFF)
        {
            /* Power Button */
            if (BTN_POWER == ON)
            {
                Timer0_Delay1ms(HOLD_TIME);
                if( BTN_POWER == ON)
                {
                    NumControlFan = NUM_SPEED_L;
                    IsActiveRelay = ON;
                    while(BTN_POWER == ON);
                }
            }
        }
        else
        {
            /* Power Button */
            if ( BTN_POWER == ON )
            {
                Timer0_Delay1ms(HOLD_TIME);
                if( BTN_POWER == ON)
                {
                    NumControlFan = NUM_SPEED_DEFAULT;
                    IsActiveRelay = OFF;
                    while(BTN_POWER == ON);
                }
            }

            /* UP Button */
            if ( BTN_UP == ON )
            {
                Timer0_Delay1ms(5);
                if ( BTN_UP == ON )
                {
                    NumControlFan++;
                    if ( NumControlFan > NUM_SPEED_H )
                    {
                        NumControlFan = NUM_SPEED_H;
                    }
                    while(BTN_UP == ON);
                }
            }

            /* DOWN Button */
            if (BTN_DOWN == ON)
            {
                Timer0_Delay1ms(5);
                if( BTN_DOWN == ON)
                {
                    NumControlFan--;
                    if (NumControlFan < NUM_SPEED_L )
                    {
                        NumControlFan = NUM_SPEED_L;
                    }
                    while(BTN_DOWN == ON);
                }
            }
        }
				
        /* Main task system */
        arr_Speed_Func[NumControlFan]();
				#endif
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/* Init GPIO                                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
void InitGPIO(void)
{
    /* POWER */
    P12_Input_Mode;
    /* UP */
    P10_Input_Mode;
    /* DOWN */
    P00_Input_Mode;
    /* RELAY SPEED L */
    P17_PushPull_Mode;
    /* RELAY SPEED M */
    P15_PushPull_Mode;
    /* RELAY SPEED H */
    P01_PushPull_Mode;
}
/*---------------------------------------------------------------------------------------------------------*/
/* System Sumilink                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
void Set_RL_Speed_1(void)
{
    SPEED_L = ON;
    SPEED_M = OFF;
    SPEED_H = OFF;
}
void Set_RL_Speed_2(void)
{
    SPEED_L = OFF;
    SPEED_M = ON;
    SPEED_H = OFF;
}
void Set_RL_Speed_3(void)
{
    SPEED_L = OFF;
    SPEED_M = OFF;
    SPEED_H = ON;
}
void Set_RL_FAN_Default(void)
{
    SPEED_L = OFF;
    SPEED_M = OFF;
    SPEED_H = OFF;
}
