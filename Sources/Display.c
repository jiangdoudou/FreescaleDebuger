#include "Display.h"

extern int CursorLine;

void PrintLCD(char *str, uint8 xPos, uint8 yPos)
{

    int num, end;
    SetDisplayPosition(xPos, yPos);
    end = strlen(str) - yPos;
    for (num = 0; num < end && num < 16; num++)
    {
        LCD_WriteData(str[num]);
    }
}


void SetRealCursor(uint8 line)
{
    SetDisplayPosition(line, 0);
    CursorLine = line;
}

uint8  GetRealCursor()
{
    return CursorLine;
}



void PrintStringsToLCD(DisplayInfo *info, int offset)
{
    int i = offset;
    int j = 0;
    int n = info->size;
    ClearScreen();
    for(; i < n && j < 4 ; i++, j++)
    {
        PrintLCD((info->words)[i], j, 0);
    }
    for(;j<4;j++)
    {
    	PrintLCD("  ", j, 0);	
    }
}

void InitDisplay(DisplayInfo *info)
{
	
    PrintStringsToLCD(info, 0);
    info->curPos = 0;
    SetRealCursor(0);
}

void CursorMoveDown(DisplayInfo *info)
{
    int i = GetRealCursor();
    int j;
    info->curPos++;
    info->curPos %= info->size;
    if(i == 3||info->curPos==0)
    {
    	if(info->size > 4)
    	{
    		
    		j=4*(info->curPos/4);
    		PrintStringsToLCD(info, j);
    	}
        SetRealCursor(0);
    }
    else
    {
        SetRealCursor(i + 1);
    }
}
void CursorMoveUp(DisplayInfo *info)
{
    int i = GetRealCursor();
    int j;
	info->curPos--;
	info->curPos+=info->size;
	info->curPos%=info->size;
    if(i == 0)
    {
    	if(info->size > 4) 
    	{
    		j=4*(info->curPos/4);
			PrintStringsToLCD(info, j);
    	}
		SetRealCursor(info->curPos%4);
    }
    else
    {
        SetRealCursor(i - 1);
    }
}
void PageUp(DisplayInfo *info)
{
    int curPage,allPage,j;
    curPage=(info->curPos)/4; 
    allPage=(info->size-1)/4+1;
	if(allPage==1) return ;
	curPage--;
	curPage+=allPage;
	curPage%=allPage;
	j=curPage*4;
    PrintStringsToLCD(info,j);
	info->curPos=j+info->curPos%4;
	if(info->curPos>=info->size)  info->curPos = info->size -1;
	SetRealCursor(info->curPos%4);
    
}
void PageDown(DisplayInfo *info)
{

    int curPage,allPage,j;
    curPage=(info->curPos)/4;
    allPage=(info->size-1)/4+1;
	if(allPage==1) return ;
	curPage++;
	curPage%=allPage;
	j=curPage*4;
	info->curPos=j+info->curPos%4;
	if(info->curPos>=info->size)  info->curPos = info->size -1;
	PrintStringsToLCD(info,j);
	SetRealCursor(info->curPos%4);
}

void ResumeDisplay(DisplayInfo *info)
{
    int curPage,allPage;
    int j;
    curPage=(info->curPos)/4;
    j=(curPage)*4;	
    PrintStringsToLCD(info,j);
    SetRealCursor(info->curPos%4);			
}
