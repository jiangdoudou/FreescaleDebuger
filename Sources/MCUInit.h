//--------------------------------------------------------------------------*
// 文件名: MCUInit.h                                                        *
// 说  明: 系统初始化头文件                                                 *
//--------------------------------------------------------------------------*

#ifndef  MCUINIT_H
#define  MCUINIT_H
    
    //头文件
    #include "headers.h"     //包含总头文件
    
    //宏定义
    //设定系统总线时钟频率    
    #define FBUS_16M 16
    #define FBUS_20M 20
    #define FBUS_32M 32
    #define FBUS_40M 40
    #define FBUS_48M 48
    #define FBUS_60M 60 


    //函数声明
    void MCUInit(uint8 fbus);        //芯片初始化

#endif