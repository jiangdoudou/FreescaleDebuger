#include "headers.h"

//��ʼ������
void InitKeyboard(void)
{
    GPPort_Set(PH, PTI, 1, 0xFF);     //��λ��Ӧ�Ĵ���
    GPPort_Set(PH, DDR, 0, 0xE0);     //����PH[4:0]Ϊ����
    GPPort_Set(PAD1, PRT, 0, 0xF0);   //��λ��Ӧ�Ĵ���
    GPPort_Set(PAD1, DDR, 1, 0x0F);   //����PAD1[3:0]Ϊ���
    GPPort_Set(PAD0, PRT, 0, 0x00);   //��λ��Ӧ�Ĵ���
    GPPort_Set(PAD0, DDR, 1, 0xFF);   //����PAD0[1]Ϊ���
    PERH = 0xFF;
    PPSH = 0xE0;
    PIEH = 0xFF;
}

//ʹ�ܼ���
void EnableKeyboard(void)
{
    PIEH = 0xFF;
}

//���ü���
void DisableKeyboard(void)
{
    PIEH = 0x00;
}

//���ؼ�ֵ
Keys GetKey(void)
{
    return keyValue;
}

//������ֵ
float GetValue(void)
{
    return numValue;
}

void KeyboardProcess(void)
{
	Delay_ms(5);
	if(!PTIH)
	{
		return;
	}
	
    DDRA = 0xFF;
    PORTA = 0x00;

    if (0 == Row0)
    {
        keyValue = 0;
    }
    else if (0 == Row1)
    {
        keyValue = 5;
    }
    else if (0 == Row2)
    {
        keyValue = 10;
    }
    else if (0 == Row3)
    {
        keyValue = 15;
    }
    else if (0 == Row4)
    {
        keyValue = 20;
    }
    else
    {
        //Error;
    }

    //����PHΪ�����PADΪ���룬����к�
    //DisableKeyboard();
    GPPort_Set(PH, PRT, 0, 0xE0);     //��λ��Ӧ�Ĵ���
    GPPort_Set(PH, DDR, 1, 0x1F);     //����PH[4:0]Ϊ���
    GPPort_Set(PAD1, PRT, 1, 0xFF);   //��λ��Ӧ�Ĵ���
    GPPort_Set(PAD1, DDR, 0, 0xF0);   //����PAD1[3:0]Ϊ����
    GPPort_Set(PAD0, PRT, 1, 0xFF);   //��λ��Ӧ�Ĵ���
    GPPort_Set(PAD0, DDR, 0, 0xFD);   //����PAD0[1]Ϊ����
    ATD0DIEN = 0x020F;
    Delay_us(10);

    if (0 == Column0)
    {
        //keyValue += 0;
    }
    else if (0 == Column1)
    {
        keyValue += 1;
    }
    else if (0 == Column2)
    {
        keyValue += 2;
    }
    else if (0 == Column3)
    {
        keyValue += 3;
    }
    else if (0 == Column4)
    {
        keyValue += 4;
    }
    else
    {
        //Error;
    }

    //EnableKeyboard();
    InitKeyboard();
    PIFH = 0xFF;              //����ն˱�־λ
}