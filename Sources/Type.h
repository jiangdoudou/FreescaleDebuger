//--------------------------------------------------------------------------*
// �ļ���: Type.h (�������ͱ����ļ�)                                        *
// ˵  ��: ����������͵ı���,Ŀ��:                                         *
//         (1)���̱���������д����;                                         *
//         (2)���������ֲ,���Ը�����Ҫ�������.                            *
//--------------------------------------------------------------------------*

#ifndef TYPE_H
#define TYPE_H

typedef unsigned char         uint8;             //  8 λ�޷�����
typedef unsigned short int    uint16;            // 16 λ�޷�����
typedef unsigned long int     uint32;            // 32 λ�޷�����
typedef char                  int8;              //  8 λ�з�����
typedef short int             int16;             // 16 λ�з�����
typedef int                   int32;             // 32 λ�з�����
typedef unsigned int          uint;

//���Ż���������,�ؼ���volatile
typedef volatile uint8        vuint8;            //  8 λ�޷�����
typedef volatile uint16       vuint16;           // 16 λ�޷�����
typedef volatile uint32       vuint32;           // 32 λ�޷�����
typedef volatile int8         vint8;             //  8 λ�з�����
typedef volatile int16        vint16;            // 16 λ�з�����
typedef volatile int32        vint32;            // 32 λ�з�����
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

