#ifndef	_LCD_H_
#define	_LCD_H_

#include "headers.h"

//���Ŷ���
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
// ����: void LCD_WriteData(unsigned int Data)
// ����: дһ���ֽڵ���ʾ������LCD�е���ʾ����RAM����
// ����: Data д�������
// ����: ��
// ��ע: ��
//========================================================================
void LCD_WriteData(uchar Data);



//========================================================================
// ����: void LCD_WriteReg(uchar command)
// ����: дһ���ֽڵ�������LCD�еĿ��ƼĴ�������
// ����: command д������ݣ��Ͱ�λ��Ч��byte��
// ����: ��
// ��ע: ��
//========================================================================
void LCD_WriteReg(uchar command);

//========================================================================
// ����: void LCD_Init()
// ����: LCD��ʼ����������������LCD��ʼ����Ҫ���õ����Ĵ������������
// �û����˽⣬����鿴DataSheet���и����Ĵ���������
// ����: ��
// ����: ��
// ��ע:
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