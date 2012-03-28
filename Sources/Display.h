#ifndef _DISPLAY_H_
#define _DISPLAY_H_
#include "LCD.h"
#include "headers.h"

typedef struct DisplayInfo
{
    char **words; //Ҫ��ʾ������
    volatile int curPos;//��ǰ���λ��
    int size;//������
} DisplayInfo;
void PrintLCD(char *str, uint8 xPos, uint8 yPos);
void InitDisplay(DisplayInfo *info); //��ʼ��
void CursorMoveUp(DisplayInfo *info);//�������
void CursorMoveDown(DisplayInfo *info);//�������
void PageUp(DisplayInfo *info);
void PageDown(DisplayInfo *info);
void ResumeDisplay(DisplayInfo *info);


#endif	  //_DISPLAY_H_