#ifndef _GETCONTENT_H_
#define _GETCONTENT_H_

#include "headers.h"


int getParameter(char *filePath, char names[][17], char values[][17]);
int setParameter(char *filePath, char names[][17], char values[][17], int n);
uint16 getBackupFiles(char names[][17]);
void getConstData(float *PLUS,float *MINUS,float *MUL,float *DIV);
void setConstData(float PLUS,float MINUS,float MUL,float DIV);
int initSDFAT();
uint16 getLogFiles(char names[][17]);

#endif