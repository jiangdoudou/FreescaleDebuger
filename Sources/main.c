#include "headers.h"

Keys keyValue=Null;
unsigned char Dev_No = 0;
char t[24];
void main(void)
{
    EnableInterrupts;
    MCUInit(FBUS_32M);
    InitGpio();
    LCD_Init();    
    InitKeyboard();
    EnableKeyboard();
    ShowMainMenu();
}