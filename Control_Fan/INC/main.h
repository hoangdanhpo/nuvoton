#include "MS51.h"
#include "Common.h"
#include "Delay.h"
#include "SFR_Macro.h"
#include "Function_define.h"

/* Pin define */
#define KEYCARD_PIN    				P11
#define FAN_PIN        				P10
#define WINDOWN_SW_PIN        P00
#define RELAY_PIN        			P03

/* FAN & RELAY Status define */
#define OFF    				1
#define ON    				0

/* KEYCARD Status define */
#define NOT_INSERTED    		1
#define INSERTED    				0

/* WINDOWN_SW Status define */
#define NOT_CLOSED    		1
#define CLOSED    				0

/* Uart1 */
#define Uart_TX_PIN        P16
#define Uart_RX_PIN        P02

