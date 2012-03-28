#include "headers.h"

//初始化键盘
void InitKeyboard(void)
{
    GPPort_Set(PH, PTI, 1, 0xFF);     //复位相应寄存器
    GPPort_Set(PH, DDR, 0, 0xE0);     //设置PH[4:0]为输入
    GPPort_Set(PAD1, PRT, 0, 0xF0);   //复位相应寄存器
    GPPort_Set(PAD1, DDR, 1, 0x0F);   //设置PAD1[3:0]为输出
    GPPort_Set(PAD0, PRT, 0, 0x00);   //复位相应寄存器
    GPPort_Set(PAD0, DDR, 1, 0xFF);   //设置PAD0[1]为输出
    PERH = 0xFF;
    PPSH = 0xE0;
    PIEH = 0xFF;
}

//使能键盘
void EnableKeyboard(void)
{
    PIEH = 0xFF;
}

//禁用键盘
void DisableKeyboard(void)
{
    PIEH = 0x00;
}

//返回键值
Keys GetKey(void)
{
    return keyValue;
}

//返回数值
float GetValue(void)
{
    return numValue;
}

void KeyboardProcess(void)
{
	Delay_ms(5);
	if(!PTIH)
	{
		return;
	}
	
    DDRA = 0xFF;
    PORTA = 0x00;

    if (0 == Row0)
    {
        keyValue = 0;
    }
    else if (0 == Row1)
    {
        keyValue = 5;
    }
    else if (0 == Row2)
    {
        keyValue = 10;
    }
    else if (0 == Row3)
    {
        keyValue = 15;
    }
    else if (0 == Row4)
    {
        keyValue = 20;
    }
    else
    {
        //Error;
    }

    //设置PH为输出，PAD为输入，检测列号
    //DisableKeyboard();
    GPPort_Set(PH, PRT, 0, 0xE0);     //复位相应寄存器
    GPPort_Set(PH, DDR, 1, 0x1F);     //设置PH[4:0]为输出
    GPPort_Set(PAD1, PRT, 1, 0xFF);   //复位相应寄存器
    GPPort_Set(PAD1, DDR, 0, 0xF0);   //设置PAD1[3:0]为输入
    GPPort_Set(PAD0, PRT, 1, 0xFF);   //复位相应寄存器
    GPPort_Set(PAD0, DDR, 0, 0xFD);   //设置PAD0[1]为输入
    ATD0DIEN = 0x020F;
    Delay_us(10);

    if (0 == Column0)
    {
        //keyValue += 0;
    }
    else if (0 == Column1)
    {
        keyValue += 1;
    }
    else if (0 == Column2)
    {
        keyValue += 2;
    }
    else if (0 == Column3)
    {
        keyValue += 3;
    }
    else if (0 == Column4)
    {
        keyValue += 4;
    }
    else
    {
        //Error;
    }

    //EnableKeyboard();
    InitKeyboard();
    PIFH = 0xFF;              //清除终端标志位
}