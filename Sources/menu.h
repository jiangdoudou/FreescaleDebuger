#ifndef _MENU_H_
#define _MENU_H_
#include "headers.h"


void ShowMainMenu();
void ParaMenu();
void ComMenu();
void ConstMenu();
void BacklistMenu();
void BackdeleteMenu();
void BackfileOpen(int i);
char* ValueEdit(char *src);
char* TextInput(int xPos,int yPos,char *d);
int ShowMessage(char msg1[17],char msg2[17],int delaytime);





#endif