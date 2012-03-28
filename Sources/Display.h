#ifndef _DISPLAY_H_
#define _DISPLAY_H_
#include "LCD.h"
#include "headers.h"

typedef struct DisplayInfo
{
    char **words; //要显示的内容
    volatile int curPos;//当前光标位置
    int size;//总行数
} DisplayInfo;
void PrintLCD(char *str, uint8 xPos, uint8 yPos);
void InitDisplay(DisplayInfo *info); //初始化
void CursorMoveUp(DisplayInfo *info);//光标上移
void CursorMoveDown(DisplayInfo *info);//光标下移
void PageUp(DisplayInfo *info);
void PageDown(DisplayInfo *info);
void ResumeDisplay(DisplayInfo *info);


#endif	  //_DISPLAY_H_