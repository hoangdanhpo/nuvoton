  /******************************************************************************
  * @file           : oled_i2c.h
  * @brief          : Oled programming
	By Hoang Danh Nguyen - Email: hoangdanhpo@gmail.com
  ******************************************************************************/

#define u8 unsigned char 
#define u32 unsigned int 

/*SCL    P1.3
SDA      P1.4*/
#define OLED_SCLK_Clr() SCL=0
#define OLED_SCLK_Set() SCL=1
#define OLED_SDIN_Clr() SDA=0
#define OLED_SDIN_Set()	SDA=1


#define SYS_CLK_EN              0
#define SYS_SEL                 2
#define SYS_DIV_EN              0                   //0: Fsys=Fosc, 1: Fsys = Fosc/(2*CKDIV)
#define SYS_DIV                 1
#define I2C_CLOCK               2
#define TEST_OK                 0x00


#define OLED_CMD  0	// Command
#define OLED_DATA 1	// Data
#define OLED_MODE 0
#define SIZE 16
#define XLevelL		0x02
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF 
#define X_WIDTH 	128
#define Y_WIDTH 	64	

void OLED_WR_Byte(unsigned dat,unsigned cmd);  
void OLED_Display_On(void);
void OLED_Display_Off(void);                                                  
void OLED_Init(void);
void OLED_Clear(void);
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 Char_Size);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size);
void OLED_ShowString(u8 x,u8 y, u8 *p,u8 Char_Size); 
void OLED_ShowFloatNum(u8 x,u8 y,double v_floatNum_f32,u8 Float_Size);
void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]);
void fill_picture(unsigned char fill_Data);
void Init_I2C(void);
void IIC_Start();
void IIC_Stop();
void Write_IIC_Command(unsigned char IIC_Command);
void Write_IIC_Data(unsigned char IIC_Data);
void Write_IIC_Byte(unsigned char IIC_Byte);
void IIC_Wait_Ack();