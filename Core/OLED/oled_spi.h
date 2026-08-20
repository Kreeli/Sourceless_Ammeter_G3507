#ifndef __OLED_SPI__
#define __OLED_SPI__
#include <stdlib.h>
#include "ti_msp_dl_config.h"
#define OLED_CMD 0  // 命令
#define OLED_DATA 1 // 数据
#define OLED_MODE 0

#define  u8  unsigned char 
#define  u32 unsigned int 


//-----------------OLED驱动方式说明----------------
// 使用SPI通信驱动OLED屏,速度快
#define SPI_OLED_INST SPI_0_INST

//-----------------OLED端口定义----------------
// 引脚宏在 ti_msp_dl_config.h 里(SysConfig 生成的), 每个引脚单独 PORT

#define OLED_RES_Clr()  DL_GPIO_clearPins(OLED_RES_PORT,OLED_RES_PIN)//RES
#define OLED_RES_Set()  DL_GPIO_setPins(OLED_RES_PORT,OLED_RES_PIN)

#define OLED_DC_Clr()   DL_GPIO_clearPins(OLED_DC_PORT,OLED_DC_PIN)//DC
#define OLED_DC_Set()   DL_GPIO_setPins(OLED_DC_PORT,OLED_DC_PIN)

#define OLED_CS_Clr()   DL_GPIO_clearPins(OLED_CS_PORT,OLED_CS_PIN)//CS
#define OLED_CS_Set()   DL_GPIO_setPins(OLED_CS_PORT,OLED_CS_PIN)

#define OLED_Off()				DL_GPIO_clearPins(OLED_VCC_PORT,OLED_VCC_PIN)// 断电
#define OLED_On()			DL_GPIO_setPins(OLED_VCC_PORT,OLED_VCC_PIN)//上电
// OLED模式设置
// 0:4线串行模式
// 1:并行8080模式

#define SIZE 16
#define XLevelL 0x02
#define XLevelH 0x10
#define Max_Column 128
#define Max_Row 64
#define Brightness 0xFF
#define X_WIDTH 128
#define Y_WIDTH 64


// OLED驱动函数
void OLED_WR_Byte(u8 dat,u8 cmd);	  
void delay_ms(unsigned int ms);
void delay_us(unsigned int us);
void OLED_Display_On(void);
void OLED_Display_Off(void);	   							   		    
void OLED_Init(void);
void OLED_Clear(void);
void OLED_DrawPoint(u8 x,u8 y,u8 t);
void OLED_Refresh(void);
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);
void OLED_ShowChar(u8 x,u8 y,char chr);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size2);
void OLED_ShowString(u8 x,u8 y, char *p);	 
void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_ShowCHinese(u8 x,u8 y,u8 no);
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,const unsigned char BMP[]);

#endif
