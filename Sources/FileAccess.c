#include "FileAccess.h"
#include "headers.h"
/**************************************************************************
 - ������������һ���ļ�
 - �������ԣ��ⲿ��ʹ�û�ʹ��
 - ����˵����pfi: FileInfoStruct���͵Ľṹ��ָ�룬����װ���ļ��Ĳ�����Ϣ
              �����ļ��Ĵ�С���ļ������ơ��ļ��Ŀ�ʼ�صȵȣ��Ա�����ʹ��
             filepath: �ļ���·����֧�������Ŀ¼ ����
              "\\dir1\\dir2\\dir3\\....\\test.txt"
			 Method��0 ֻ����1����д���ߴ���
 - ����˵����0���ɹ� 1���ļ������� 2��Ŀ¼������ 
 - ע�����ļ����ɹ��кܶ�ԭ�򣬱����ļ������ڡ��ļ���ĳһ��Ŀ¼������
 **************************************************************************/
UINT8 Open(struct FileInfoStruct *pfi , INT8 *filepath , int Method)
{
    UINT8 ptd[3];
    if(Method == READONLY)
    {
        return FAT32_Open_File(pfi, filepath, 0);
    }
    else if(Method == WRITE)
    {
		 FAT32_Del_File(filepath);
         return FAT32_Create_File(pfi, filepath, ptd);
    }
}

/**************************************************************************
 - �������������ļ���ĳһ��λ�ô�����ȡһ���ֽ�
 - �������ԣ��ⲿ��ʹ�û�ʹ��
 - ����˵����pfi:FileInfoStruct���͵Ľṹ��ָ�룬����װ���ļ�������Ϣ���ļ�
             ��ȡ�Ĺ����У��˽ṹ���е���ز�������£������ļ��ĵ�ǰƫ������
             �ļ��ĵ�ǰ�������ļ��ĵ�ǰ�صȵ�
             offset:��ȡ����ְ
             pbuf:���ݻ�����
 - ����˵������ȡ����ʵ�ʵ����ݳ��ȣ������ȡʧ�ܣ�����ָ����ƫ�����������ļ�
             ��С���򷵻�0
 - ע���ڶ�ȡһ���ļ�������ǰ�������Ƚ����ļ���
 **************************************************************************/
UINT8 ReadOneByteFromFile(struct FileInfoStruct *pfi, UINT32 offset, INT8 *pbuf)
{
    return FAT32_Read_File(pfi, offset, 1, (UINT8 *)pbuf);
}

/**************************************************************************
 - �������������ļ���ĳһ��λ�ô�����ȡһ�����ȵ����ݣ��������ݻ�������
 - �������ԣ��ⲿ��ʹ�û�ʹ��
 - ����˵����pfi:FileInfoStruct���͵Ľṹ��ָ�룬����װ���ļ�������Ϣ���ļ�
             ��ȡ�Ĺ����У��˽ṹ���е���ز�������£������ļ��ĵ�ǰƫ������
             �ļ��ĵ�ǰ�������ļ��ĵ�ǰ�صȵ�
             offset:Ҫ��λ��ƫ������ҪС���ļ��Ĵ�С
             len:Ҫ��ȡ�����ݵĳ��ȣ����len+offset�����ļ��Ĵ�С����ʵ�ʶ�
             ȡ���������Ǵ�offset��ʼ���ļ�����
             pbuf:���ݻ�����
 - ����˵������ȡ����ʵ�ʵ����ݳ��ȣ������ȡʧ�ܣ�����ָ����ƫ�����������ļ�
             ��С���򷵻�0
 - ע���ڶ�ȡһ���ļ�������ǰ�������Ƚ����ļ���
 **************************************************************************/
UINT8 ReadBytesFromFile(struct FileInfoStruct *pfi, UINT32 offset, UINT32 len, INT8 *pbuf)
{
    return FAT32_Read_File(pfi, offset, len, (UINT8 *)pbuf);
}

/**************************************************************************
 - ������������ĳһ���ļ�׷������
 - �������ԣ��ⲿ��ʹ�û�ʹ��
 - ����˵����pfi:ָ��FileInfoStruct���͵Ľṹ�壬����װ�ظմ������ļ�����Ϣ
             pbuf:ָ�����ݻ�������ָ��
 - ����˵�����ɹ�����ʵ��д������ݳ��ȣ�ʧ�ܷ���0
 - ע��׷������ʧ�ܺ��п����Ǵ洢�豸�Ѿ�û�пռ��ˣ�Ҳ�����Ҳ������д���
 **************************************************************************/
UINT32 WriteOneByteToFile(struct FileInfoStruct *pfi, INT8 *pbuf)
{
    return FAT32_Add_Dat(pfi, 1, (UINT8 *)pbuf);
}

/**************************************************************************
 - ������������ĳһ���ļ�׷������
 - �������ԣ��ⲿ��ʹ�û�ʹ��
 - ����˵����pfi:ָ��FileInfoStruct���͵Ľṹ�壬����װ�ظմ������ļ�����Ϣ
             len:Ҫ׷�ӵ����ݳ���
             pbuf:ָ�����ݻ�������ָ��
 - ����˵�����ɹ�����ʵ��д������ݳ��ȣ�ʧ�ܷ���0
 - ע��׷������ʧ�ܺ��п����Ǵ洢�豸�Ѿ�û�пռ��ˣ�Ҳ�����Ҳ������д���
 **************************************************************************/
UINT32 WriteBytesToFile(struct FileInfoStruct *pfi, UINT32 len, INT8 *pbuf)
{
    return FAT32_Add_Dat(pfi, len , (UINT8 *)pbuf);
}

/**************************************************************************
 - ����������ɾ���ļ�(֧�������Ŀ¼)
 - �������ԣ��ⲿ��ʹ�û�ʹ��
 - ����˵����filepath:�ļ�·�� ���� "\\dir1\\dir2\\dir3\\....\\dirn\\test.txt"
 - ����˵����1:�ļ���Ŀ¼·�������� 0:�ɹ�
 - ע��ɾ������ļ���FAT���еĴ�����ϵ��ȫ���ƻ�
 **************************************************************************/
UINT8 DeleteFile(INT8 *filepath)
{
    return FAT32_Del_File(filepath);
}

/**************************************************************************
 - �����������ļ��ر�
 - �������ԣ��ⲿ��ʹ�û�ʹ��
 - ����˵����pfi:ָ��ǰ�򿪵��ļ����ļ���Ϣ�ṹ
 - ����˵����0:�ɹ�
 - ע����
 **************************************************************************/
UINT8 CloseFile(struct FileInfoStruct *pfi)
{
    return FAT32_File_Close(pfi);
}

