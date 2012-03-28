#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include "headers.h"

//�궨��
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

//ö�ٶ���
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

//ȫ�ֱ�������
extern Keys keyValue;
extern float numValue;

//��������
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif

#endif /* __cplusplus */

    //��ʼ������
    void InitKeyboard(void);

    //ʹ�ܼ���
    void EnableKeyboard(void);

    //���ü���
    void DisableKeyboard(void);

    //���ؼ�ֵ
    enum Keys GetKey(void);

    //������ֵ
    float GetValue(void);

    //�����жϴ���
    void KeyboardProcess(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */



#endif    //_KEYBOARD_H_