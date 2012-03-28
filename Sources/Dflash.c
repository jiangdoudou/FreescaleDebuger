#include "Dflash.h"

#define READword(address)     ((unsigned int)(*(volatile unsigned int *__near)(address)))

#define DFLASH_LOWEST_START_PAGE        0x00        //����data flash����ʼҳ
#define DFLASH_START                    0x00100000  //����data flash����ʼ��ַ
#define DFLASH_PAGE_SIZE                0x0400      //����data flash�Ĵ�СΪ1K.
#define DFLASH_PAGE_WINDOW_START        0x0800      //����data flashҳ�洰�ڵ���ʼ��ַ

/*************************************************************/
/*                      ��ʼ��DFLASH                         */
/*************************************************************/
void DFlash_Init(void)
{
   while(FSTAT_CCIF==0);            //�ȴ����ڴ����FLASH�������
   FCLKDIV=0x0F;                    //�ⲿ����Ϊ16M.FLASHʱ�Ӳ�����1M����������ֲ�
   FCNFG=0x00;                      //��ֹ�ж�
   while(FCLKDIV_FDIVLD==0);        //�ȴ�ʱ�����óɹ�
}

/*************************************************************/
/*                     ��DFLASHд������                      */
/*************************************************************/
void DFlash_Write(word ADDR16,word d1,word d2)
{
    while(FSTAT_CCIF==0); 
    if(FSTAT_ACCERR)           //�жϲ������־λ��
        FSTAT_ACCERR=1;
    if(FSTAT_FPVIOL)           //�жϲ������־λ��
        FSTAT_FPVIOL=1;
    FCCOBIX_CCOBIX=0x00; 
    FCCOB=0x1110;         //д������͸�λ��ַ
    FCCOBIX_CCOBIX=0x01;  //��ַ��16λ
    FCCOB=ADDR16;         //д���16λ��ַ
    FCCOBIX_CCOBIX=0x02;  //д���һ������
    FCCOB=d1;
    FCCOBIX_CCOBIX=0x03;  //д���һ������
    FCCOB=d2;
      
    FSTAT_CCIF=1;         //д��ִ������
    while(FSTAT_CCIF==0); //�ȴ�ִ�����
}

/*************************************************************/
/*                     ��DFLASH��ȡ����                      */
/*************************************************************/
word DFlash_Read (word destination)
{
    byte   lastepage;          //���ڴ洢EPAGE��ֵ
    byte   epage;              //���ڼ���EPAGE��ֵ
    unsigned int data;         //��ȡ��������

    lastepage = EPAGE;   //����EPAGE��ֵ

    epage = (byte)((DFLASH_LOWEST_START_PAGE)+(destination >>10));   //����EPAGE
    EPAGE=epage;                                                     //��EPAGE��ֵ
 
    data = READword((destination & (DFLASH_PAGE_SIZE - 1)) + DFLASH_PAGE_WINDOW_START);  //��ȡҳ�洰���е�����

    EPAGE= lastepage;       //�ָ�EPAGE��ֵ

    return(data);
}

/*************************************************************/
/*                    ����DFLASH��һ������                   */
/*************************************************************/
void DFlash_Erase(word ADDR16)
{   
  while(FSTAT_CCIF==0);
  if(FSTAT_ACCERR)           //�жϲ������־λ��
      FSTAT_ACCERR=1;
  if(FSTAT_FPVIOL)           //�жϲ������־λ��
      FSTAT_FPVIOL=1;
  
  FCCOBIX_CCOBIX=0x00;
  FCCOB=0x1210;           //д���������͸�λ��ַ
  FCCOBIX_CCOBIX=0x01;   
  FCCOB=ADDR16;           //д���16λ�ĵ�ַ
  FSTAT_CCIF=1;           //����ִ������
  while(FSTAT_CCIF==0);   //�ȴ�ִ�����
}