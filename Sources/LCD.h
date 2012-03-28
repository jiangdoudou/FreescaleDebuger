#ifndef	_LCD_H_
#define	_LCD_H_

#include "headers.h"

//引脚定义
//#define	LCD_CS		PORTK_PK5
#define LCD_RST		PORTK_PK4
#define LCD_PSB		PORTK_PK0
#define LCD_EP		PORTK_PK1
#define LCD_RW		PORTK_PK3
#define LCD_A0		PORTK_PK2

#define	LCD_DATA	PORTB


#define LCD_READ_FLAG	    1
#define LCD_WRITE_FLAG	  0
#define LCD_DATA_FLAG	    1
#define LCD_COMMAND_FLAG	0

//void lcd_pos(uchar m);
void InitGpio();
void SetBusCLK_48M(void);
void CheckBusy();

//========================================================================
// 函数: void LCD_WriteData(unsigned int Data)
// 描述: 写一个字节的显示数据至LCD中的显示缓冲RAM当中
// 参数: Data 写入的数据
// 返回: 无
// 备注: 无
//========================================================================
void LCD_WriteData(uchar Data);



//========================================================================
// 函数: void LCD_WriteReg(uchar command)
// 描述: 写一个字节的数据至LCD中的控制寄存器当中
// 参数: command 写入的数据，低八位有效（byte）
// 返回: 无
// 备注: 无
//========================================================================
void LCD_WriteReg(uchar command);

//========================================================================
// 函数: void LCD_Init()
// 描述: LCD初始化程序，在里面会完成LCD初始所需要设置的许多寄存器，具体如果
// 用户想了解，建议查看DataSheet当中各个寄存器的意义
// 参数: 无
// 返回: 无
// 备注:
//========================================================================
void LCD_Init(void);
uchar LCD_ReadData(void);
void SetDisplayPosition(uchar x, uchar y);
void ClearScreen(void);
void SetDisplayState(uchar x, uchar y, uchar z);
//void DisplayOpp(uchar a,uchar b,uchar c,uchar d);
void DisplayMov(uchar a, uchar n);
void HelloWorld();
void Flash(uchar x, uchar y);
//void drawpoint(uchar wx,uchar wy,uchar sel);


#endif	//	_LCD_H_