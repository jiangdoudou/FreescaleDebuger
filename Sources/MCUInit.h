//--------------------------------------------------------------------------*
// �ļ���: MCUInit.h                                                        *
// ˵  ��: ϵͳ��ʼ��ͷ�ļ�                                                 *
//--------------------------------------------------------------------------*

#ifndef  MCUINIT_H
#define  MCUINIT_H
    
    //ͷ�ļ�
    #include "headers.h"     //������ͷ�ļ�
    
    //�궨��
    //�趨ϵͳ����ʱ��Ƶ��    
    #define FBUS_16M 16
    #define FBUS_20M 20
    #define FBUS_32M 32
    #define FBUS_40M 40
    #define FBUS_48M 48
    #define FBUS_60M 60 


    //��������
    void MCUInit(uint8 fbus);        //оƬ��ʼ��

#endif