#ifndef _DFLASH_H_
#define _DFLASH_H_
#include "headers.h"
void DFlash_Init(void);  
void DFlash_Write(word ADDR16,word d1,word d2);
word DFlash_Read (word destination);
void DFlash_Erase(word ADDR16); 
#endif