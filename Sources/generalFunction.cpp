#include "headers.h"


void Delay_us(uint time)
//ÑÓÊ±×Óº¯Êý
{
    while(time--)
    {
        _asm(nop);
        _asm(nop);
        _asm(nop);
        _asm(nop);
        _asm(nop);
    }
}


void Delay_10us(uint time)
{
    int x;
    while(time--)
        for(x = 0; x < 26; x++)
        {
            _asm(nop);
            _asm(nop);
            _asm(nop);
            _asm(nop);
            _asm(nop);
            _asm(nop);
        }
}

void Delay_ms(uint time)
{
    int x;
    while(time--)
        for(x = 0; x < 1000; x++)
        {
            _asm(nop);
            _asm(nop);
            _asm(nop);
            _asm(nop);
            _asm(nop);
            _asm(nop);
            _asm(nop);
            _asm(nop);
            _asm(nop);
            _asm(nop);
            _asm(nop);
            _asm(nop);
            _asm(nop);
            _asm(nop);
            _asm(nop);
            _asm(nop);
            _asm(nop);
            _asm(nop);
            _asm(nop);
            _asm(nop);
            _asm(nop);
            _asm(nop);
            _asm(nop);
            _asm(nop);
            _asm(nop);
            _asm(nop);
            _asm(nop);
            _asm(nop);
            _asm(nop);
            _asm(nop);
            _asm(nop);
            _asm(nop);
            _asm(nop);
            _asm(nop);
        }
}

void init2DArray(char ***target, char src[][17], int n)
{
    *target = (char **)malloc(n * sizeof(char *));
    for(int i = 0; i < n; i++)
    {
        (*target)[i] = (char *)malloc(17 * sizeof(char));
        strcpy((*target)[i], (src)[i]);
    }
}

void init2DEmptyArray(char ***target, int n)
{

    *target = (char **)malloc(n * sizeof(char *));
    for(int i = 0; i < n; i++)
    {
        (*target)[i] = (char *)malloc(17 * sizeof(char));
    }
}
void free2DArray(char ***target, int n)
{

    	for(int i = 0; i < n; i++)
    	{
    		free((*target)[i]);
    		(*target)[i]=NULL;
    	}
    	free(*target);
    	(*target)=NULL;
}
char* itoa(long value,char *str,long radix)  
{  
    long sign = 0;    
    char ps[32];   
    long i=0;  
    if(value < 0)  
    {  
        sign = -1;  
        value = -value;  
    }  
    do  
    {  
        if(value%radix>9)  
            ps[i] = value%radix +'0'+7;  
        else   
            ps[i] = value%radix +'0';  
        i++;  
    }while((value/=radix)>0);  
    if(sign<0)  
        ps[i] = '-';  
    else  
        i--;  
    for(int j=i;j>=0;j--)  
    {  
        str[i-j] = ps[j];  
    }
    str[++i]='\0';  
    return str;  
}

void f32toi16(float src,word *d1,word *d2)
{
	float *p=&src;
	*d1=*((word *)p);
	*d2=*( (word *)p +1);
}
void i16tof32(float *d,word s1,word s2)
{
	word *d1=(word *)d;
	word *d2=d1+1;
	*d1=s1;
	*d2=s2;
}


