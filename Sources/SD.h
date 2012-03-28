#ifndef _SD_H
#define _SD_H

#include "headers.h"

void SPI_High_Rate(void) ;
void Init_Dev(void);
void SD_Init(void);
byte Wt_Block(dword sector, byte *buffer);
byte Rd_Block(dword sector, byte *buffer);
byte SD_Reset(void);
void New_Buf8(byte buffer[], byte ch);
void CLS_Buf8(byte buffer[]);
void View_Buf8(byte buffer[]);

#endif