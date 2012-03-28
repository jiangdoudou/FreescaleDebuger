#include "headers.h"

int getZero(float value)
{
	float t=0.1;
	int i=0;
	if(value < 1e-6) return 0;
	while(1)
	{
		if(value>t) return i;
		t*=0.1;
		i++;
	}
}
char * insertZero(char *str,int n)
{
	char temp[17];
	int i,j;
	if(n==0)  return str;
	for(i=0;i<n;i++)
	{
		temp[i]='0';
	}
	for(j=0;str[j]!='\0';j++)
	{
		temp[i++]=str[j];
	}
	temp[i]='\0';
	strcpy(str,temp);
	return str;
}



char * ftoa(float number,int ndigit,char *buf)  
{  
    volatile long int_part;  
    volatile float float_part;  
    volatile char str_int[17];  
    volatile char str_float[17];
    volatile long i;    
    int_part = (long)number;  
    float_part = number - int_part;  
    // 整数部分处理   
    itoa(int_part,str_int,10);  
    // 小数部分处理   
    if(ndigit>0)  
    {  
        long t=getZero(float_part);
        float_part = fabs(pow(10, ndigit) * float_part);
        itoa((long)float_part,str_float,10);
        insertZero(str_float,t);  
    }
    i=strlen(str_int);
    str_int[i++]='.';
    str_int[i]='\0';  
    strcat(str_int,str_float);  
    strcpy(buf,str_int);  
    return buf;  
}  		


int strlenNum(char *str)  //浮点数近似
{
    int i = 0;
	int l=0;
    for(; str[i] != '\0' && str[i] != '.'; i++);
	l=strlen(str);
	if(l>i+3) return i+4;
	else return l;
}
void strcatn(char src1[][17], char src2[][17] , int n)
{
    int k = 0;
	int i=0;
    for(i = 0; i < n; i++)
    {
        int j =strlen(src1[i]);
        int space = 16 - j - strlenNum(src2[i]);
        for(k = 0; k < space; k++)
        {
            src1[i][j++] = ' ';
        }
        for(k = 0; j < 16; k++, j++)
        {
            src1[i][j] = src2[i][k];
        }
        src1[i][j] = '\0';

    }
}

void getMaxString(char src[][17],char dest[17],int n)
{
	int i;
	strcpy(dest,src[0]);
	for(i=1;i<n;i++)
	{
		if(strcmp(dest,src[i])<0)
		{
			strcpy(dest,src[i]);
		}
	}
}

void dropFilenamePost(char *str)
{
	int i=0;
	for(i=0;str[i]!='.';i++)
	{
		
	}
	str[i]='\0';
}
char* FilenameInc(char *str)
{
	int i,l;
	int x=atoi(str);
	x++;
	itoa(x,str,10);
	l=strlen(str);
	if(l<4)
	{
		insertZero(str,4-l);
	}
	return str;
}

char *Calculate(char *str,int method)
{
	float PLUS,MINUS,MUL,DIV;
	volatile float dest=atof(str);
	
	getConstData(&PLUS,&MINUS,&MUL,&DIV);
	switch(method)
	{
		case Multiply:
			{
				dest*=MUL;
			    break;
			}
			case Devide:
			{
				dest/=DIV;
				break;	
			}
			case Plus:
			{
				dest+=PLUS;
				break;
			}
			case Minus:
			{
				dest-=Minus;
				break;	
			}			
	}
	ftoa(dest,5,str);
	str[16]='\0';
	return str;
}