#include "menu.h"

volatile char names[MAX_PARANUM][17];
volatile char values[MAX_PARANUM][17];
char filename[40];
void ParaMenu()
{
    int isEdit = TRUE;   
	volatile int paraNumbers;
	DisplayInfo *paraMenu=(DisplayInfo *)malloc(sizeof(DisplayInfo));
    paraMenu->curPos = 0;   
    for(;;)
    {
        if(isEdit)
        {
        	paraNumbers = getParameter(FILE_PATH, names, values);
            paraMenu->size = paraNumbers;       
            strcatn(names, values, paraNumbers);
            init2DArray(&(paraMenu->words), names, paraNumbers);
            ResumeDisplay(paraMenu);
            isEdit = FALSE;
        }
        switch(keyValue)
        {
        case Up:
        {
            keyValue = Null;
            CursorMoveUp(paraMenu);
            break;
        }
        case Down:
        {
            keyValue = Null;
            CursorMoveDown(paraMenu);
            break;
        }
        case Left:
       	{
        	keyValue=Null;
        	PageUp(paraMenu);
        	break;	
       	}
       	case Right:
       	{
        	keyValue=Null;
        	PageDown(paraMenu);
        	break;
       	}
        case Enter:
        {	
        	int choice = paraMenu->curPos;
        	char temp[17];
        	keyValue = Null;           
            isEdit = TRUE;
            strcpy(temp,values[choice]);
            ValueEdit(temp);
            if(strcmp(temp,values[choice])!=0)
            {
            	strcpy(filename,"\\backup\\");
            	getParameter(FILE_PATH,names,values);
            	strcat(filename,names[choice]);
            	strcat(filename,values[choice]);
            	strcat(filename,".txt");           	
            	setParameter(filename,names,values,paraNumbers); 
            	strcpy(values[choice],temp);
            	setParameter(FILE_PATH,names,values,paraNumbers);           	
            }
            free2DArray(&(paraMenu->words),paraNumbers);
            break; 
        }
        case Backspace:
        {
        	keyValue = Null;
			free2DArray(&(paraMenu->words),paraNumbers); 
			free(paraMenu);
            return;
        }
        default:
        {
            break;
        }
        }
    }	
	
}