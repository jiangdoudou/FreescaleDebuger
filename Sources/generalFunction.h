#ifndef	_GENERAL_FUNCTION_H_
#define	_GENERAL_FUNCTION_H_

#include "headers.h"

//将寄存器某一位置1
#define BSET(bit,Register)     ((Register)|= (1<<(bit)))
//将寄存器某一位清零
#define BCLR(bit,Register)     ((Register) &= ~(1<<(bit)))
//获取寄存器某一位状态
#define BGET(bit,Register)     (((Register) >> (bit)) & 1)


#define  TRUE 1
#define  FALSE 0


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */


    void Delay_us(uint time);
    void Delay_10us(uint time);
    void Delay_ms(uint time) ;
    void init2DArray(char ***target, char src[][17], int n);
    void strcatn(char src1[][17], char src2[][17] , int n);
    char* itoa(long value,char *str,long radix) ;
    char* ftoa(float number,int ndigit,char *buf) ;
    void f32toi16(float src,word *d1,word *d2);
	void i16tof32(float *d,word s1,word s2);
	void init2DEmptyArray(char ***target, int n);
	void free2DArray(char ***target, int n);
	void getMaxString(char src[][17],char dest[17],int n);
	void dropFilenamePost(char *str);
	char* FilenameInc(char *str);
	char *Calculate(char *str,int method);
    


#ifdef __cplusplus
#if __cplusplus
}
#endif

#endif /* __cplusplus */

#endif	//	_GENERAL_FUNCTION_H_