#include "menu.h"

char ConstStr[4][17] = {"自加常量", "自减常量", "自乘常量", "自除常量"};
void ConstMenu()
{
    int choice;
    float PLUS,MINUS,MUL,DIV;
    DisplayInfo *constMenu=(DisplayInfo *)malloc(sizeof(DisplayInfo));
    getConstData(&PLUS,&MINUS,&MUL,&DIV);   
    init2DArray(&(constMenu->words), ConstStr, 4);
    constMenu->curPos = 0;
    constMenu->size = 4;
    InitDisplay(constMenu);
    for(;;)
    {
        switch(keyValue)
        {
        case Up:
        {
            keyValue = Null;
            CursorMoveUp(constMenu);
            break;
        }
        case Down:
        {
            keyValue = Null;
            CursorMoveDown(constMenu);
            break;
        }
        case Enter:
        {
            keyValue = Null;
            choice = constMenu->curPos;
            
            switch(choice)
            {
               	char s[17];
        		
            	case 0: PLUS=atof(ValueEdit(ftoa(PLUS,5,s)));break;
            	case 1: MINUS=atof(ValueEdit(ftoa(MINUS,5,s)));break;
            	case 2: MUL=atof(ValueEdit(ftoa(MUL,5,s)));break;
            	case 3: DIV=atof(ValueEdit(ftoa(DIV,5,s)));break;        
        		
        	}
            setConstData(PLUS,MINUS,MUL,DIV);
            ResumeDisplay(constMenu);
            break;

        }
        case Backspace:
        {
        	keyValue = Null;
        	free2DArray(&(constMenu->words),4);
        	free(ConstMenu);
            return;
        }
        default:
        {
            break;
        }
        }

    }
	

}