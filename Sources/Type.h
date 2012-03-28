//--------------------------------------------------------------------------*
// 文件名: Type.h (变量类型别名文件)                                        *
// 说  明: 定义变量类型的别名,目的:                                         *
//         (1)缩短变量类型书写长度;                                         *
//         (2)方便程序移植,可以根据需要自行添加.                            *
//--------------------------------------------------------------------------*

#ifndef TYPE_H
#define TYPE_H

typedef unsigned char         uint8;             //  8 位无符号数
typedef unsigned short int    uint16;            // 16 位无符号数
typedef unsigned long int     uint32;            // 32 位无符号数
typedef char                  int8;              //  8 位有符号数
typedef short int             int16;             // 16 位有符号数
typedef int                   int32;             // 32 位有符号数
typedef unsigned int          uint;

//不优化变量类型,关键字volatile
typedef volatile uint8        vuint8;            //  8 位无符号数
typedef volatile uint16       vuint16;           // 16 位无符号数
typedef volatile uint32       vuint32;           // 32 位无符号数
typedef volatile int8         vint8;             //  8 位有符号数
typedef volatile int16        vint16;            // 16 位有符号数
typedef volatile int32        vint32;            // 32 位有符号数
typedef volatile uint         vuint;

#define UINT8X  unsigned char xdata
#define UINT8   unsigned char
#define UINT16X unsigned int xdata
#define UINT16  unsigned int
#define UINT32X unsgined long xdata
#define UINT32  unsigned long

#define INT8X   char xdata
#define INT8    char
#define INT16X  int  xdata
#define INT16   int
#define INT32X  long xdata
#define INT32   long

#endif

