#include "menu.h"
char MainStr[4][17] = {"�鿴����", "���ݽ���", "�����趨", "���ݹ���"};
void ShowMainMenu()
{
    int choice;
    DisplayInfo *mainMenu=(DisplayInfo *)malloc(sizeof(DisplayInfo));
    mainMenu->curPos = 0;
    mainMenu->size = 4;
    init2DArray(&(mainMenu->words), MainStr, mainMenu->size);
    InitDisplay(mainMenu);
    for(;;)
    {
        switch(keyValue)
        {
        case Up:
        {
            keyValue = Null;
            CursorMoveUp(mainMenu);
            break;
        }
        case Down:
        {
            keyValue = Null;
            CursorMoveDown(mainMenu);
            break;
        }
        	
        case Enter:
        {
        	keyValue = Null;
            choice = mainMenu->curPos;        
            switch(choice)
            {
            
            case 0:ParaMenu();break;
            case 1:ComMenu();break;
            case 2:ConstMenu();break;
            case 3:BacklistMenu();break;
            }
            ResumeDisplay(mainMenu);
            break;
        }
        default:
        {
            break;
        }
        }
    }
    free2DArray(&(mainMenu->words),mainMenu->size);
	free(mainMenu);
}