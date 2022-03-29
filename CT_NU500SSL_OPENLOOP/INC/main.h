#include "MS51.h"
#include "Common.h"
#include "Delay.h"
#include "SFR_Macro.h"
#include "Function_define.h"

/* Hall sensor */
#define Hall_sensor_A      P30
#define Hall_sensor_B      P16
#define Hall_sensor_C      P15

/* PWM */
#define PWM_H_A 					 P12
#define PWM_L_A 					 P11
#define PWM_H_B 					 P10
#define PWM_L_B 					 P00
#define PWM_H_C 					 P01	
#define PWM_L_C 					 P03

/* ADC */
#define Cur_Measure_PIN    P05          // ADC CH5
#define Vol_Measure_PIN    P04          // ADC CH1
#define Target_Speed_PIN   P17          // ADC CH0

/* DIR */
#define DIR_PIN            P15          // CW/CCW 

/* OCP/DIS Function */
#define OSP_DIS_PIN        P14

/* Pulse Signal Output */
#define Pulse_Output_PIN   P13

/* Brake Function */
#define BRK_PIN            P20

/* Uart1 */
#define Uart_TX_PIN        P16
#define Uart_RX_PIN        P02

