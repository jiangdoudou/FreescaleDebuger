#include "menu.h" 

char backfilesStr[MAX_BACKUPNUM][17];
char backnames[MAX_BACKUPNUM][17];
char backvalues[MAX_BACKUPNUM][17];
extern char filename[40];
void BacklistMenu()
{
    int choice;
    int backFileCount;
    int isChange=TRUE;
    DisplayInfo *backlistMenu=(DisplayInfo *)malloc(sizeof(DisplayInfo));
    backlistMenu->curPos = 0;    
    for(;;)
    {
    	if(isChange)
    	{
    		backFileCount = getBackupFiles(backfilesStr);    
		    if(backFileCount==0)
		    {
		    	ClearScreen();
		        PrintLCD("No backup Files", 0, 0);
				Delay_ms(1000);
		        return ;	
		    }
		    init2DArray(&(backlistMenu->words), backfilesStr, backFileCount);		    
		    backlistMenu->size = backFileCount;
		    if(backlistMenu->curPos>=backlistMenu->size) 
		    	backlistMenu->curPos=backlistMenu->size-1;
		    ResumeDisplay(backlistMenu);
		    isChange=FALSE;	
    	}
    	
        switch(keyValue)
        {
        case Up:
        {
            keyValue = Null;
            CursorMoveUp(backlistMenu);
            break;
        }
        case Down:
        {
            keyValue = Null;
            CursorMoveDown(backlistMenu);
            break;
        }
        case Left:
       	{
        	keyValue=Null;
        	PageUp(backlistMenu);
        	break;	
       	}
       	case Right:
       	{
        	keyValue=Null;
        	PageDown(backlistMenu);
        	break;
       	}
        case Enter:
        {
            keyValue = Null;
            choice = backlistMenu->curPos;
            BackfileOpen(choice);
            isChange=TRUE;
            free2DArray(&(backlistMenu->words),backFileCount);
            break;

        }
        case Backspace:
        {
        	keyValue = Null;
        	free2DArray(&(backlistMenu->words),backFileCount);
        	free(backlistMenu);
            return;
        }
        default:
        {
            break;
        }
        }
    }
	
}
void BackdeleteMenu()
{
    //确认是否删除界面
}
void BackfileOpen(int i)
{
    int backparaNumbers;
	DisplayInfo *backparaMenu=(DisplayInfo *)malloc(sizeof(DisplayInfo));
	strcpy(filename,"\\backup\\");
	strcat(filename,backfilesStr[i]);
    backparaNumbers = getParameter(filename, backnames, backvalues);
    strcatn(backnames, backvalues, backparaNumbers);
    init2DArray(&(backparaMenu->words), backnames, backparaNumbers);
    backparaMenu->curPos = 0;
    backparaMenu->size = backparaNumbers;
    InitDisplay(backparaMenu);
    for(;;)
    {
        switch(keyValue)
        {
        case Up:
        {
            keyValue = Null;
            CursorMoveUp(backparaMenu);
            break;
        }
        case Down:
        {
            keyValue = Null;
            CursorMoveDown(backparaMenu);
            break;
        }
        case Left:
       	{
        	keyValue=Null;
        	PageUp(backparaMenu);
        	break;	
       	}
       	case Right:
       	{
        	keyValue=Null;
        	PageDown(backparaMenu);
        	break;
       	}
        case Backspace:
        {
        	keyValue = Null;
			free2DArray(&(backparaMenu->words), backparaNumbers);
			free(backparaMenu);
            return;
        }
        case Enter:
        {
            keyValue = Null;
            if(ShowMessage("是否还原备份?","enter/back",0))
            {
            	getParameter(filename, backnames, backvalues);
	            setParameter(FILE_PATH,backnames,backvalues,backparaNumbers);
	            free2DArray(&(backparaMenu->words), backparaNumbers);
	            return ;
            }
            break;

        }
        case Clear:
        {
        	keyValue = Null;
        	if(ShowMessage("是否删除?","enter/back",0)) 
        	{
        		DeleteFile(filename);
        		free2DArray(&(backparaMenu->words), backparaNumbers);
        		return ;
        	}
        	break;
        }
        default:
        {
            break;
        }
        }
    }
	
}