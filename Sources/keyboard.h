#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include "headers.h"

//宏定义
#define Column0 PT1AD0_PT1AD03
#define Column1 PT1AD0_PT1AD02
#define Column2 PT1AD0_PT1AD01
#define Column3 PT1AD0_PT1AD00
#define Column4 PT0AD0_PT0AD01
#define Row0    PTH_PTH2
#define Row1    PTH_PTH0
#define Row2    PTH_PTH4
#define Row3    PTH_PTH3
#define Row4    PTH_PTH1

//枚举定义
typedef enum
{
    Clear = 0,
    Space = 1,
    Menu = 2,
    Backspace = 3,
    Null = 4,
    num_7 = 5,
    num_8 = 6,
    num_9 = 7,
    Plus = 8,
    Minus = 9,
    num_4 = 10,
    num_5 = 11,
    num_6 = 12,
    Multiply = 13,
    Devide = 14,
    num_1 = 15,
    num_2 = 16,
    num_3 = 17,
    Up = 18,
    Enter = 19,
    num_0 = 20,
    Point = 21,
    Left = 22,
    Down = 23,
    Right = 24
} Keys;

//全局变量声明
extern Keys keyValue;
extern float numValue;

//函数声明
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif

#endif /* __cplusplus */

    //初始化键盘
    void InitKeyboard(void);

    //使能键盘
    void EnableKeyboard(void);

    //禁用键盘
    void DisableKeyboard(void);

    //返回键值
    enum Keys GetKey(void);

    //返回数值
    float GetValue(void);

    //键盘中断处理
    void KeyboardProcess(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */



#endif    //_KEYBOARD_H_