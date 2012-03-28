#include "headers.h"

/*
//========================================================================
//设置时钟
//========================================================================

void SetBusCLK_48M(void)
{
  CLKSEL = 0x00;
  PLLCTL_PLLON = 1;
  SYNR = 0xC0|0x05;
  REFDV = 0x80|0x01;
  POSTDIV = 0x00;
  _asm(nop);
  _asm(nop);
  while(0 == CRGFLG_LOCK);
  CLKSEL_PLLSEL = 1;
} */


//判忙
void CheckBusy()
{
    DDRB = 0x00;
    asm(nop);
    asm(nop);
    LCD_A0 = LCD_COMMAND_FLAG;
    LCD_RW = LCD_READ_FLAG;
    LCD_EP = 1;
    Delay_ms(1);
    while((LCD_DATA & 0x80) == 0x80);
    LCD_EP = 0;
    DDRB = 0xFF;
}


//========================================================================
// 函数: void LCD_WriteReg(uchar command)
// 描述: 写一个字节的数据至LCD中的控制寄存器当中
// 参数: command 写入的数据，低八位有效（byte）
// 返回: 无
// 备注: 无
//========================================================================
void LCD_WriteReg(uchar command)
{
    CheckBusy();
    LCD_A0 = LCD_COMMAND_FLAG;
    LCD_RW = LCD_WRITE_FLAG;
    LCD_DATA = command;
    Delay_ms(1);
    LCD_EP = 1;
    Delay_ms(1);
    LCD_EP = 0;

}

//========================================================================
// 函数: void LCD_WRiteData(unsigned int Data)
// 描述: 写一个字节的显示数据至LCD中的显示缓冲RAM当中
// 参数: Data 写入的数据
// 返回: 无
// 备注: 无
//========================================================================
void LCD_WriteData(uchar Dat)
{
    CheckBusy();
    LCD_A0 = LCD_DATA_FLAG;
    LCD_RW = LCD_WRITE_FLAG;
    LCD_DATA = Dat;
    Delay_ms(1);
    LCD_EP = 1;
    Delay_ms(1);
    LCD_EP = 0;
}

//========================================================================
// 函数: uchar LCD_ReadData()
// 描述: 从LCD中的显示缓冲RAM当中读一个字节的显示数据
// 参数: 无
// 返回: 读出的数据
// 备注: 无
//========================================================================
uchar LCD_ReadData(void)
{
    uchar Read_Data;

    CheckBusy();
    LCD_A0 = LCD_DATA_FLAG;
    LCD_RW = LCD_READ_FLAG;
    LCD_EP = 1;
    Read_Data = LCD_DATA;
    Delay_ms(1);
    LCD_EP = 0;
    return Read_Data;
}




/*void lcd_pos(uchar m)//地址行选择
{
   uchar  pos;
   if (m==0)
     {m=0x80;}
   else if (m==1)
     {m=0x90;}
   else if (m==2)
     {m=0x88;}
   else if (m==3)
     {m=0x98;}

   pos = m;

   LCD_WriteReg(m);     //显示地址
}      */
//========================================================================
// 函数: void LCD_Init()
// 描述: LCD初始化程序，在里面会完成LCD初始所需要设置的许多寄存器，具体如果
// 用户想了解，建议查看DataSheet当中各个寄存器的意义
// 参数: 无
// 返回: 无
// 备注:
//========================================================================
void InitGpio(void)
{
    GPPort_Set(PK, DDR, 1, 0xFF);
    GPPort_Set(PB, DDR, 1, 0xFF);
    //GPPort_Set(PORTK_PK0, PRT, 1, 1);
    PORTK_PK7 = 1;
}



void LCD_Init(void)
{
    LCD_PSB = 1;
    LCD_RST = 0;
    Delay_ms(20);

    LCD_RST = 1;

    LCD_WriteReg(0x30); 	//8位数据 使用基本指令
    Delay_ms(5);
    LCD_WriteReg(0x0e);		//开显示：无游标，不反白
    Delay_ms(1);
    LCD_WriteReg(0x01);		//清屏
    Delay_ms(5);
    ;

}

//========================================================================
//显示位置：从第零行，第零列开始。
//========================================================================
void SetDisplayPosition(uchar x, uchar y)
{
    switch(x)
    {
    case 0:
        LCD_WriteReg(0x80 + y );
        break;
    case 1:
        LCD_WriteReg(0x90 + y );
        break;
    case 2:
        LCD_WriteReg(0x88 + y );
        break;
    case 3:
        LCD_WriteReg(0x98 + y );
        break;
    default:
        break;
    }

}

//========================================================================
//显示字符串
//========================================================================
/*void LCD_PutString(uchar x,uchar y,uchar *s)
{
   switch(x)
  {
    case 1: LCD_WriteReg(0x80+y);break;
    case 2: LCD_WriteReg(0x90+y);break;
    case 3: LCD_WriteReg(0x88+y);break;
    case 4: LCD_WriteReg(0x98+y);break;
      default:break;
  }
  while(*s>0)
  {
    LCD_WriteData(*s);
    s++;
    Delay_10us(5);
  }
}*/

//========================================================================
//清屏
//========================================================================
void ClearScreen(void)
{
    LCD_WriteReg(0x01);
    Delay_ms(5);
}
//========================================================================
//显示状态：x为整体显示开关，y为游标开关，z为游标显示开关。1为开，0为关
//========================================================================
void SetDisplayState(uchar x, uchar y, uchar z)
{
    uchar ctrl = 0x08;
    ctrl += (x << 2);
    ctrl += (y << 1);
    ctrl += z;

    LCD_WriteReg(ctrl);
}

//========================================================================
//设置反白显示行 1为反白 0为正常
/*//========================================================================
void DisplayOpp(uchar a,uchar b,uchar c,uchar d)
{
     LCD_WriteReg(0x34);

     if(a == 1)
     {
         LCD_WriteReg(0x04);
     }
     if(b == 1)
     {
         LCD_WriteReg(0x05);
     }
     if(c == 1)
     {
         LCD_WriteReg(0x06);
     }
     if(d == 1)
     {
         LCD_WriteReg(0x07);
     }


}  */
//========================================================================
//显示左右移  1为右移，0为左移 ,n为次数
//========================================================================
void DisplayMov(uchar a, uchar n)
{
    uchar x;
    if (a == 1)
    {
        for(x = 0; x < n; x++)
        {
            LCD_WriteReg(0x18);
        }
    }
    if (a == 0)
    {
        for(x = 0; x < n; x++)
        {
            LCD_WriteReg(0x1C);
        }
    }
}
//========================================================================
//显示HelloWorld~!@_@
//========================================================================
void HelloWorld()
{
    uchar num;
    uchar str[] = "HelloWorld";
    for(num = 0; num < 12; num++)
    {
        LCD_WriteData(str[num]);
    }

}
//========================================================================
//在任一位置闪烁
//========================================================================
void Flash(uchar x, uchar y)
{

    SetDisplayPosition(x, y);
    LCD_WriteReg(0x30);
    LCD_WriteReg(0x0f);


}
//========================================================================
//画点
//========================================================================
/*void drawpoint(uchar wx,uchar wy,uchar sel)
{

    uchar xh,xl;
    uchar xadds,yadds;
    uchar data_h,data_l;
    xh = wx>>4;
    xl = wx % 0x0f;
    if(wy < 32)
    {
        yadds = wy;
        xadds = xh;
    }
    else
    {
        xadds = xh + 8;
        yadds = wy - 32;
    }
    LCD_WriteReg(0x34);
     LCD_WriteReg(yadds + 0x80);
      LCD_WriteReg(xadds + 0x80);
       LCD_WriteReg(0x30);
       LCD_ReadData();
        data_h = LCD_ReadData();
          data_l = LCD_ReadData();
            LCD_WriteReg(0x34);
     LCD_WriteReg(yadds + 0x80);
      LCD_WriteReg(xadds + 0x80);
      if(xl < 8)
      {
          switch(sel)
          {
            case 0 : data_h&= (~(1<<(7-xl)));break;
            case 1 : data_h|= (1<<(7-xl));break;
            default : break;
          }
      }
        else
          {
             switch(sel)
          {
            case 0 : data_l&= (~(1<<(15-xl)));break;
            case 1 : data_l|= (1<<(15-xl));break;
            default : break;
          }
          }
         LCD_WriteReg(0x30);
         	LCD_WriteData(data_h);
         		LCD_WriteData(data_l);
         		   LCD_WriteReg(0x36);
}  */

/*//========================================================================
//反白
//========================================================================
void SetWhite(uchar x,uchar y,uchar end_x,uchar clear)
{
 uchar i,j,white_x,white_y,white_end_x;
 uchar s_x,y_x,s_end_x,y_end_x;
 white_end_x = (end_x - x + 1);
 s_x = x/2;
 y_x = x%2;
 s_end_x = end_x/2;
 y_end_x = end_x%2;
 if(y_x == 1)
 {
  white_end_x++;
 }
 if(y_end_x == 0)
 {
  white_end_x++;
 }
   LCD_WriteReg(0x36);
         if(y == 0)
   {
    white_x = (0x80+x/2);
    white_y = 0x80;
   }
    else if(y == 1)
    {
      white_x = (0x80+x/2);
      white_y = 0x90;
    }
    else if(y == 2)
   {
    white_x = (0x88+x/2);
    white_y = 0x80;
   }
    else if(y == 3)
    {
      white_x = (0x88+x/2);
      white_y = 0x90;
    }

  if(clear == 0)
  {
    for (i = 0;i < 32; i++)
    {
        LCD_WriteReg(0x80+i);
        LCD_WriteReg(0x80);
        for(j = 0;j < 32;j++)
        {
           	LCD_WriteData(0x00);
           _asm(nop);

        }
    }
  }

  _asm(nop);
   for (i = 0;i < 16; i++)
   {
        LCD_WriteReg(white_y++);
        LCD_WriteReg(white_x);
        for(j = 0;j < white_end_x;j++)
        {
          if((y_x == 1)&&(j == 0))
          {
             	LCD_WriteData(0x00);
          }
          else
          {
            if((y_end_x == 0)&&(j == (white_end_x-1)))
            {
              	LCD_WriteData(0x00);
            }
          else
          {
            	LCD_WriteData(0xff);
          }
          }
        }
   }
     LCD_WriteReg(0x30);
}
   */




/*LCD_WriteReg(0xAF);		//LCD On
	TimeDelay(20);
LCD_WriteReg(0x2F);		//设置上电控制模式
	TimeDelay(20);
LCD_WriteReg(0x81);		//电量设置模式（显示亮度）
	TimeDelay(20);
LCD_WriteReg(0x1F);		//指令数据0x0000~0x003f		亮度值
	TimeDelay(20);
LCD_WriteReg(0x27);		//V5内部电压调节电阻设置？？？
	TimeDelay(20);
LCD_WriteReg(0xA2);		//LCD偏压设置？？？
	TimeDelay(20);
LCD_WriteReg(0xC8);		//Com 扫描方式设置,反向？？？
	TimeDelay(20);
LCD_WriteReg(0xA0);		//Segment方向选择,正常？？？
	TimeDelay(20);
LCD_WriteReg(0xA4);		//全屏点亮/变暗指令
	TimeDelay(20);
LCD_WriteReg(0xA6);		//正向反向显示控制指令
	TimeDelay(20);
LCD_WriteReg(0xAC);		//关闭静态指示器？？？
	TimeDelay(20);
LCD_WriteReg(0x00);		//指令数据
	TimeDelay(20);
LCD_WriteReg(0x40 + 32);	//设置显示起始行对应RAM
	TimeDelay(20);
LCD_WriteReg(0xE0);		//设置读写改模式
	TimeDelay(20);  */