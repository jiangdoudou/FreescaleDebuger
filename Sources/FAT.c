#include "headers.h"
#include "string.h"

//ȫ�ֱ�������
struct direntry  temp_rec;
INT8 temp_dir_name[20];
INT8 temp_file_name[20];
UINT32  temp_dir_cluster;
UINT32  temp_last_cluster;
unsigned char (*pRS)(unsigned long, unsigned char *); //ָ��ʵ�ʴ洢�豸�Ķ����������ĺ���ָ�룬����ʵ�ֶ��豸��֧��
unsigned char (*pWS)(unsigned long, unsigned char *); //ָ��ʵ�ʴ洢�豸��д���������ĺ���ָ�룬����ʵ�ֶ��豸��֧��
FileInfoStruct fi;
UINT8 FAT32_Buffer[512]; //�������ݶ�д������,���ⲿ�ṩ

/******************************************************************
 - ����������znFAT�Ĵ洢�豸�ײ������ӿڣ���ȡ�洢�豸��addr������
             512���ֽڵ����ݷ���buf���ݻ�������
 - ����ģ�飺znFAT�ļ�ϵͳģ��
 - �������ԣ��ڲ���������洢�豸�ĵײ������Խӣ�
 - ����˵����addr:������ַ
             buf:ָ�����ݻ�����
 - ����˵����0��ʾ��ȡ�����ɹ�������ʧ��
 - ע����������������Ǿ������ϵ����ִ洢�豸����SD������Ч����U�̡�
       CF��ͨ���ڳ����ж�̬���л���ͬ���豸�������Ӷ�ʵ�ֶ��豸(��ͬ
	   ʱ�Զ��ִ洢�豸���в����������SD�������ļ���U�̵ȵ�)����ͬ
	   �������л���ֻ��Ҫ�ڳ����иı�Dev_No���ȫ�ֱ�����ֵ����
 ******************************************************************/

UINT8 FAT32_ReadSector(UINT32 addr, UINT8 *buf)
{
    switch(Dev_No)
    {
    case SDCARD:
        pRS = Rd_Block;
        break;
    }
    return (*pRS)(addr, buf); //�滻��ʵ�ʴ洢����������������������SD������������
}

/******************************************************************
 - ����������znFAT�Ĵ洢�豸�ײ������ӿڣ���buf���ݻ������е�512��
             �ֽڵ�����д�뵽�洢�豸��addr������
 - ����ģ�飺znFAT�ļ�ϵͳģ��
 - �������ԣ��ڲ���������洢�豸�ĵײ������Խӣ�
 - ����˵����addr:������ַ
             buf:ָ�����ݻ�����
 - ����˵����0��ʾ��ȡ�����ɹ�������ʧ��
 - ע����
 ******************************************************************/

UINT8 FAT32_WriteSector(UINT32 addr, UINT8 *buf)
{
    switch(Dev_No)
    {
    case SDCARD:
        pWS = Wt_Block;
        break;
    }
    return (*pWS)(addr, buf); //�滻��ʵ�ʴ洢��������д������������SD������д����
}

/******************************************************************
 - ����������С��ת��ˣ���LittleEndian��BigEndian
 - ����ģ�飺znFAT�ļ�ϵͳģ��
 - �������ԣ��ڲ�
 - ����˵����dat:ָ��ҪתΪ��˵��ֽ�����
             len:ҪתΪ��˵��ֽ����г���
 - ����˵����תΪ���ģʽ���ֽ���������������
 - ע�����磺С��ģ���       0x33 0x22 0x11 0x00  (���ֽ���ǰ)
             תΪ���ģʽ��Ϊ 0x00 0x11 0x22 0x33  (���ֽ���ǰ)
             ��������ֵΪ   0x00112233
             (CISC��CPUͨ����С�˵ģ�����FAT32Ҳ���ΪС�ˣ�����Ƭ��
              ����RISC��CPU��ͨ����˵���Ǵ�˵ģ�������Ҫ�����������
              �ڵĴ�Ŵ�����е��������ܵõ���ȷ����ֵ)
 ******************************************************************/

UINT32 LE2BE(UINT8 *dat, UINT8 len)
{
    UINT32 temp = 0;
    UINT32 fact = 1;
    UINT8  i = 0;
    for(i = 0; i < len; i++)
    {
        temp += dat[i] * fact;
        fact *= 256;
    }
    return temp;
}

/******************************************************************
 - ������������С���ַ�תΪ��д
 - ����ģ�飺znFAT�ļ�ϵͳģ��
 - �������ԣ��ڲ�
 - ����˵����c:Ҫת��Ϊ��д���ַ�
 - ����˵����Ҫת�����ֽڵ���Ӧ�Ĵ�д�ַ�
 - ע��ֻ��Сд�ַ���Ч���������a~z��Сд�ַ�����ֱ�ӷ���
 ******************************************************************/

INT8 L2U(INT8 c)
{
    if(c >= 'a' && c <= 'z') return c + 'A' - 'a';
    else return c;
}

/******************************************************************
 - ������������ȡ0�����������û��MBR(��������¼)
 - ����ģ�飺znFAT�ļ�ϵͳģ��
 - �������ԣ��ڲ�
 - ����˵������
 - ����˵����1��ʾ��⵽MBR��0��ʾû�м�⵽MBR
 - ע����Щ�洢�豸��ʽ��ΪFAT32�Ժ�û��MBR����0��������DBR
       �����MBR������Ҫ������н������Եõ�DBR������λ�ã�ͬʱMBR��
       ����������������������Ϣ
 ******************************************************************/

UINT8 FAT32_is_MBR()
{
    UINT8 result;
    FAT32_ReadSector(0, FAT32_Buffer);
    if(FAT32_Buffer[0] != 0xeb)
    {
        result = 1;
    }
    else
    {
        result = 0;
    }
    return result;
}

/***********************************************************************
 - �����������õ�DBR���ڵ�������(���û��MBR����DBR����0����)
 - ����ģ�飺znFAT�ļ�ϵͳģ��
 - �������ԣ��ڲ�
 - ����˵������
 - ����˵����DBR��������ַ
 - ע��DBR�а����˺ܶ����õĲ�����Ϣ�������ȷ��λDBR������λ�ã��Ǽ�Ϊ
       ��Ҫ�ģ����潫��ר�ŵĺ�����DBR���н�������ȷ����DBR��ʵ��FAT32��
       ����
 ***********************************************************************/

UINT16 FAT32_Find_DBR()
{
    UINT16 sec_dbr;
    FAT32_ReadSector(0, FAT32_Buffer);
    if(FAT32_Buffer[0] != 0xeb)
    {
        sec_dbr = LE2BE(((((struct PartSector *)(FAT32_Buffer))->Part[0]).StartLBA), 4);
    }
    else
    {
        sec_dbr = 0;
    }
    return sec_dbr;
}

/***********************************************************************
 - ������������ȡ������������
 - ����ģ�飺znFAT�ļ�ϵͳģ��
 - �������ԣ��ⲿ��ʹ�û�ʹ��
 - ����˵������
 - ����˵������������ֵ����λΪ�ֽ�
 - ע������õ�����������FAT32������������һ��С��ʵ�ʵ���������
 ***********************************************************************/

UINT32 FAT32_Get_Total_Size()
{
    UINT32 temp;
    FAT32_ReadSector(pArg->BPB_Sector_No, FAT32_Buffer);
    FAT32_ReadSector(pArg->BPB_Sector_No, FAT32_Buffer);
    temp = ((LE2BE((((struct FAT32_BPB *)(FAT32_Buffer))->BPB_TotSec32), 4))) * pArg->BytesPerSector;
    return temp;
}

/***********************************************************************
 - ������������ȡFSInfo��ȡ�����һ�����ÿ��д�
 - ����ģ�飺znFAT�ļ�ϵͳģ��
 - �������ԣ��ڲ�
 - ����˵������
 - ����˵���������һ�����ÿ��д�
 - ע��FAT32�е�FSInfo����(����1����)�м�¼�������һ�����ÿ��д�
 ***********************************************************************/

UINT32 Search_Last_Usable_Cluster()
{
    FAT32_ReadSector(1 + pArg->BPB_Sector_No, FAT32_Buffer);
    return LE2BE(((struct FSInfo *)FAT32_Buffer)->Last_Cluster, 4);
}

/***********************************************************************
 - ����������FAT32�ļ�ϵͳ��ʼ��
 - ����ģ�飺znFAT�ļ�ϵͳģ��
 - �������ԣ��ⲿ��ʹ�û�ʹ��
 - ����˵����FAT32_Init_Arg���͵Ľṹ��ָ�룬����װ��һЩ��Ҫ�Ĳ�����Ϣ��
             �Ա�����ʹ��
 - ����˵������
 - ע����ʹ��znFATǰ����������Ǳ����ȱ����õģ����ܶ������Ϣװ�뵽
       argָ��Ľṹ���У�����������С����Ŀ¼��λ�á�FAT���С�ȵȡ�
       ��Щ�������󲿷���������DBR��BPB�У���˴˺�����Ҫ������DBR�Ĳ�������
 ***********************************************************************/

void FAT32_Init()
{
    struct FAT32_BPB *bpb;

    bpb = (struct FAT32_BPB *)(FAT32_Buffer);             //�����ݻ�����ָ��תΪstruct FAT32_BPB ��ָ��

    pArg->DEV_No = Dev_No; //װ���豸��

    pArg->BPB_Sector_No   = FAT32_Find_DBR();              //FAT32_FindBPB()���Է���BPB���ڵ�������
    pArg->BPB_Sector_No   = FAT32_Find_DBR();              //FAT32_FindBPB()���Է���BPB���ڵ�������
    pArg->Total_Size      = FAT32_Get_Total_Size();        //FAT32_Get_Total_Size()���Է��ش��̵�����������λ���ֽ�
    pArg->Total_Size      = FAT32_Get_Total_Size();        //FAT32_Get_Total_Size()���Է��ش��̵�����������λ���ֽ�

    pArg->FATsectors      = LE2BE((bpb->BPB_FATSz32)    , 4); //װ��FAT��ռ�õ���������FATsectors��
    pArg->FirstDirClust   = LE2BE((bpb->BPB_RootClus)   , 4); //װ���Ŀ¼�غŵ�FirstDirClust��
    pArg->BytesPerSector  = LE2BE((bpb->BPB_BytesPerSec), 2); //װ��ÿ�����ֽ�����BytesPerSector��
    pArg->SectorsPerClust = LE2BE((bpb->BPB_SecPerClus) , 1); //װ��ÿ����������SectorsPerClust ��
    pArg->FirstFATSector  = LE2BE((bpb->BPB_RsvdSecCnt) , 2) + pArg->BPB_Sector_No; //װ���һ��FAT�������ŵ�FirstFATSector ��
    pArg->FirstDirSector  = (pArg->FirstFATSector) + (bpb->BPB_NumFATs[0]) * (pArg->FATsectors); //װ���һ��Ŀ¼������FirstDirSector��

    temp_last_cluster = Search_Last_Usable_Cluster();
    free(bpb);
}

/***********************************************************************
 - ������������ȡʣ������
 - ����ģ�飺znFAT�ļ�ϵͳģ��
 - �������ԣ��ⲿ��ʹ�û�ʹ��
 - ����˵������
 - ����˵����ʣ����������λ�ֽ�
 - ע����FSInfo�ж�ȡ���д��������Ӽ���õ�ʣ�����������λ�ֽ�
 ***********************************************************************/

UINT32 FAT32_Get_Remain_Cap()
{
    FAT32_ReadSector(1 + pArg->BPB_Sector_No, FAT32_Buffer);
    if(((struct FSInfo *)FAT32_Buffer)->Free_Cluster[0] == 0xff
            && ((struct FSInfo *)FAT32_Buffer)->Free_Cluster[1] == 0xff
            && ((struct FSInfo *)FAT32_Buffer)->Free_Cluster[2] == 0xff
            && ((struct FSInfo *)FAT32_Buffer)->Free_Cluster[3] == 0xff)
        return pArg->Total_Size;
    else
        return LE2BE(((struct FSInfo *)FAT32_Buffer)->Free_Cluster, 4) * pArg->SectorsPerClust * pArg->BytesPerSector;
}

/***********************************************************************
 - ��������������FSInfo�еĿ��ÿ��дص�����
 - ����ģ�飺znFAT�ļ�ϵͳģ��
 - �������ԣ��ڲ�
 - ����˵����PlusOrMinus:���ÿ��д�����1���1
 - ����˵������
 - ע�������ļ���׷�����ݡ�ɾ���ļ��Ȳ��������ܻ�ʹ���õĿ��д����仯
       Ҫ��ʱ����
 ***********************************************************************/

void FAT32_Update_FSInfo_Free_Clu(UINT32 PlusOrMinus)
{
    UINT32 Free_Clu = 0;
    FAT32_ReadSector(1 + pArg->BPB_Sector_No, FAT32_Buffer);

    Free_Clu = (FAT32_Get_Remain_Cap()) / (pArg->SectorsPerClust * pArg->BytesPerSector);

    if(PlusOrMinus) Free_Clu++;
    else Free_Clu--;
    //FAT32_Buffer[((cluster*4)%pArg->BytesPerSector)+0]=Last_Clu&0x000000ff;
    // FAT32_Buffer[((cluster*4)%pArg->BytesPerSector)+1]=(Last_Clu&0x0000ff00)>>8;
    //FAT32_Buffer[((cluster*4)%pArg->BytesPerSector)+2]=(Last_Clu&0x00ff0000)>>16;
    //FAT32_Buffer[((cluster*4)%pArg->BytesPerSector)+3]=(Last_Clu&0xff000000)>>24;
    ((struct FSInfo *)FAT32_Buffer)->Free_Cluster[0] = Free_Clu & 0x000000ff; //((unsigned char *)&Free_Clu)[0];
    ((struct FSInfo *)FAT32_Buffer)->Free_Cluster[1] = (Free_Clu & 0x0000ff00) >> 8; //((unsigned char *)&Free_Clu)[1];
    ((struct FSInfo *)FAT32_Buffer)->Free_Cluster[2] = (Free_Clu & 0x00ff0000) >> 16; //((unsigned char *)&Free_Clu)[2];
    ((struct FSInfo *)FAT32_Buffer)->Free_Cluster[3] = (Free_Clu & 0xff000000) >> 24; //((unsigned char *)&Free_Clu)[3];
    FAT32_WriteSector(1 + pArg->BPB_Sector_No, FAT32_Buffer);
}

/***********************************************************************
 - ��������������FSInfo�е���һ�����ÿ��дصĴغ�
 - ����ģ�飺znFAT�ļ�ϵͳģ��
 - �������ԣ��ڲ�
 - ����˵����Last_Clu:��Ҫ���µ�FSInfo�е���һ�����ÿ��дصĴغ�
 - ����˵������
 - ע��FSInfo�е���һ�����ÿ��дغſ��Ը��ļ�ϵͳһ���ο���ֱ�Ӹ����ļ�ϵͳ
       ��һ�����õĿ��д���ʲô�ط�
 ***********************************************************************/

void FAT32_Update_FSInfo_Last_Clu(UINT32 Last_Clu)
{
    FAT32_ReadSector(1 + pArg->BPB_Sector_No, FAT32_Buffer);
    ((struct FSInfo *)FAT32_Buffer)->Last_Cluster[0] = Last_Clu & 0x000000ff; //((unsigned char *)&Last_Clu)[0];
    ((struct FSInfo *)FAT32_Buffer)->Last_Cluster[1] = (Last_Clu & 0x0000ff00) >> 8; //((unsigned char *)&Last_Clu)[1];
    ((struct FSInfo *)FAT32_Buffer)->Last_Cluster[2] = (Last_Clu & 0x00ff0000) >> 16; //((unsigned char *)&Last_Clu)[2];
    ((struct FSInfo *)FAT32_Buffer)->Last_Cluster[3] = (Last_Clu & 0xff000000) >> 24; //((unsigned char *)&Last_Clu)[3];
    FAT32_WriteSector(1 + pArg->BPB_Sector_No, FAT32_Buffer);
}

/***********************************************************************
 - ���������������һ���صĴغ�
 - ����ģ�飺znFAT�ļ�ϵͳģ��
 - �������ԣ��ڲ�
 - ����˵����LastCluster:��׼�غ�
 - ����˵����LastClutster����һ�صĴغ�
 - ע�������һ�صĴغţ�����ƾ��FAT��������¼�Ĵ�����ϵ��ʵ�ֵ�
 ***********************************************************************/

UINT32 FAT32_GetNextCluster(UINT32 LastCluster)
{
    UINT32 temp;
    struct FAT32_FAT *pFAT;
    struct FAT32_FAT_Item *pFAT_Item;
    temp = ((LastCluster / 128) + pArg->FirstFATSector);
    FAT32_ReadSector(temp, FAT32_Buffer);
    pFAT = (struct FAT32_FAT *)FAT32_Buffer;
    pFAT_Item = &((pFAT->Items)[LastCluster%128]);
    return LE2BE((UINT8 *)pFAT_Item, 4);
}

/***********************************************************************
 - �����������Ƚ�Ŀ¼��
 - ����ģ�飺znFAT�ļ�ϵͳģ��
 - �������ԣ��ڲ�
 - ����˵����a:ָ��Ŀ¼��1��ָ��
             b:ָ��Ŀ¼��2��ָ��
 - ����˵�����������Ŀ¼����ͬ�ͷ���1������Ϊ0
 ***********************************************************************/

UINT8 Compare_Dir_Name(INT8 *a, INT8 *b)
{
    UINT8 i;
    for(i = 0; i < 8; i++)
    {
        if(a[i] != b[i]) return 0;
    }
    return 1;
}

/***********************************************************************
 - �����������ļ���ƥ��(֧�ִ�*?ͨ������ļ�����ƥ��)
 - ����ģ�飺znFAT�ļ�ϵͳģ��
 - �������ԣ��ڲ�
 - ����˵����pat:Դ�ļ��������Ժ�*��?ͨ��� �� *.txt �� A?.mp3�ȵ�
             name:Ŀ���ļ���
 - ����˵������������ļ���ƥ��ͷ���1������Ϊ0
 - ע������ͨ���ļ���ƥ�䣬�����������ӣ����� A*.txt �� ABC.txt��ƥ���
   ͬʱ�� ABCDE.txtҲ��ƥ��ġ��˹������ļ�ö���н����õ�������ƥ��
   �ļ�������һ���������ļ�
 ***********************************************************************/

UINT8 FilenameMatch(INT8 *pat, INT8 *name)
{
    INT16 match, ndone;
    INT8 *cpp, *cpn;
    cpp = pat;
    cpn = name;
    match = 1;
    ndone = 1;
    while(ndone)
    {
        switch (*cpp)
        {
        case '*':
            cpp++;
            cpn = strchr(cpn, *cpp);
            if(cpn == NULL)
            {
                cpn = name;
                while(*cpn) cpn++;
            }
            break;
        case '?':
            cpp++;
            cpn++;
            break;
        case 0:
            if(*cpn != 0)
                match = 0;
            ndone = 0;
            break;
        default:
            if((*cpp) == (*cpn))
            {
                cpp++;
                cpn++;
            }
            else
            {
                match = 0;
                ndone = 0;
            }
            break;
        }
    }
    return(match);
}

/***********************************************************************
 - ����������FAT32���ļ�Ŀ¼����ļ����ֶ�(8���ֽ�)��תΪ��ͨ���ļ���
             �磺ABC     MP3 ��תΪ ABC.MP3
 - ����ģ�飺znFAT�ļ�ϵͳģ��
 - �������ԣ��ڲ�
 - ����˵����dName��ָ���ļ�Ŀ¼����ļ����ֶε�ָ��
             pName��ָ��ת����ɺ���ļ���
 - ����˵������
 - ע���˺�����������FilenameMatch�������Ϳ���ʵ�ֶ��ļ���ͨ��ƥ��
 ***********************************************************************/

void FAT32_toFileName(char *dName, char *pName)
{
    char i = 7, j = 0, k = 0;
    while(dName[i--] == ' ');
    for(j = 0; j < i + 2; j++) pName[j] = L2U(dName[j]);
    pName[j++] = '.';
    i = 10;
    while(dName[i--] == ' ');
    k = j + i - 6;
    i = 0;
    for(; j < k; j++) pName[j] = dName[8+(i++)];
    pName[j] = 0;
}

/***********************************************************************
 - �������������ַ����е�Сд�ַ���תΪ��д�ַ�
 - ����ģ�飺znFAT�ļ�ϵͳģ��
 - �������ԣ��ڲ�
 - ����˵����str��ָ���ת�����ַ���
 - ����˵������
 - ע�����ļ���������£��ļ����е��ַ���ʵ���Ǵ�д�ַ���Ϊ�˷��㣬���ļ�
       ����תΪ��д
 ***********************************************************************/

void Str2Up(char *str)
{
    unsigned char len = strlen(str), i;
    for(i = 0; i < len; i++)
    {
        str[i] = L2U(str[i]);
    }
}

/**************************************************************************
 - ��������������һ��Ŀ¼
 - ����ģ�飺znFAT�ļ�ϵͳģ��
 - �������ԣ��ⲿ��ʹ�û�ʹ��
 - ����˵����path:Ŀ¼��·�� ���磺"\\dir1\\dir2\\" �����һ������\\����
 - ����˵��������Ŀ¼�Ŀ�ʼ�غţ��������Ŀ¼ʧ�ܣ�����Ŀ¼�����ڣ��򷵻�0
 - ע���˺����ɺ����FAT32_Open_File�Ⱥ������ã�����ʵ�ִ�����Ŀ¼�µ��ļ�
       �������û�����
 **************************************************************************/

UINT32 FAT32_Enter_Dir(INT8 *path)
{
    UINT32 Cur_Clust, sec_temp, iSec, iDir, Old_Clust;
    UINT8 i = 1, counter = 0, flag = 0;
    struct direntry *pDir;
    INT8 name[17];
    Cur_Clust = pArg->FirstDirClust;
    if(path[1] == 0 && path[0] == '\\') return Cur_Clust;
    else
    {
        while(path[i] != 0)
        {
            if(path[i] == '\\')
            {
                for(; counter < 8; counter++)
                {
                    name[counter] = ' ';
                }
                name[counter] = 0;
                counter = 0;

                do
                {
                    sec_temp = (SOC(Cur_Clust));
                    for(iSec = sec_temp; iSec < sec_temp + pArg->SectorsPerClust; iSec++)
                    {
                        FAT32_ReadSector(iSec, FAT32_Buffer);
                        for(iDir = 0; iDir < pArg->BytesPerSector; iDir += sizeof(struct direntry))
                        {
                            pDir = ((struct direntry *)(FAT32_Buffer + iDir));
                            if(Compare_Dir_Name(pDir->deName, name))
                            {
                                flag = 1;
                                Cur_Clust = LE2BE(pDir->deLowCluster, 2) + LE2BE(pDir->deHighClust, 2) * 65536;
                                iDir = pArg->BytesPerSector;
                                iSec = sec_temp + pArg->SectorsPerClust;
                            }
                        }
                    }
                    Old_Clust = Cur_Clust;
                }
                while(!flag && (Cur_Clust = FAT32_GetNextCluster(Cur_Clust)) != 0x0fffffff);
                if(!flag)
                {
                    temp_dir_cluster = Old_Clust;
                    strcpy(temp_dir_name, name);
                    flag = 0;
                    return 0;
                }
                flag = 0;
            }
            else
            {
                name[counter++] = (L2U(path[i]));
            }
            i++;
        }
    }
    name[counter] = 0;
    flag = 1;
    temp_dir_cluster = Cur_Clust;
    strcpy(temp_dir_name, name);
    free(pDir);
    return Cur_Clust;
}

/**************************************************************************
 - ������������һ���ļ�(֧���ļ���ͨ�䣬�� A*.txt �� *.*)
 - ����ģ�飺znFAT�ļ�ϵͳģ��
 - �������ԣ��ⲿ��ʹ�û�ʹ��
 - ����˵����pfi: FileInfoStruct���͵Ľṹ��ָ�룬����װ���ļ��Ĳ�����Ϣ
              �����ļ��Ĵ�С���ļ������ơ��ļ��Ŀ�ʼ�صȵȣ��Ա�����ʹ��
             filepath: �ļ���·����֧�������Ŀ¼ ����
              "\\dir1\\dir2\\dir3\\....\\test.txt"
			 item�����ļ�������ͨ���*��?������£�ʵ����֮ƥ����ļ�����
			 һ����item���Ǵ򿪵��ļ����������������ͨ���������ļ���6����
			 ���item=3����ô�˺����ͻ����6���ļ��а�˳���ź�Ϊ3���Ǹ�
			 �ļ�(item��Ŵ�0��ʼ)
 - ����˵����0���ɹ� 1���ļ������� 2��Ŀ¼������
 - ע�����ļ����ɹ��кܶ�ԭ�򣬱����ļ������ڡ��ļ���ĳһ��Ŀ¼������
       ͨ������������������ļ�����С��item��ֵ�ȵ�
	   ͨ������£��ļ�����û��ͨ�����item��ֵ����ȡ0�Ϳ�����
 **************************************************************************/

UINT8 FAT32_Open_File(struct FileInfoStruct *pfi, INT8 *filepath, unsigned long item)
{
    UINT32 Cur_Clust, sec_temp, iSec, iFile, iItem = 0;
    UINT8 flag = 0, index = 0, i = 0;
    struct direntry *pFile;

    while(filepath[i] != 0)
    {
        if(filepath[i] == '\\') index = i;
        i++;
    }

    if(Cur_Clust = FAT32_Enter_Dir(filepath))
    {
        Str2Up(temp_dir_name);
        do
        {
            sec_temp = SOC(Cur_Clust);
            for(iSec = sec_temp; iSec < sec_temp + pArg->SectorsPerClust; iSec++)
            {
                FAT32_ReadSector(iSec, FAT32_Buffer);
                for(iFile = 0; iFile < pArg->BytesPerSector; iFile += sizeof(struct direntry))
                {
                    pFile = ((struct direntry *)(FAT32_Buffer + iFile));
                    FAT32_toFileName(pFile->deName, temp_file_name);
                    if(FilenameMatch(temp_dir_name, temp_file_name) && pFile->deName[0] != 0xe5 && pFile->deAttributes & 0x20) //ƥ���ļ���
                    {
                        if(item == iItem)
                        {
                            flag = 1;
                            Cur_Clust = LE2BE(pFile->deLowCluster, 2) + LE2BE(pFile->deHighClust, 2) * 65536;

                            pfi->FileSize = LE2BE(pFile->deFileSize, 4);
                            strcpy(pfi->FileName, temp_file_name);
                            pfi->FileStartCluster = LE2BE(pFile->deLowCluster, 2) + LE2BE(pFile->deHighClust, 2) * 65536;
                            pfi->FileCurCluster = pfi->FileStartCluster;
                            pfi->FileCurSector = SOC(pfi->FileStartCluster);
                            pfi->FileCurPos = 0;
                            pfi->FileCurOffset = 0;
                            pfi->Rec_Sec = iSec;
                            pfi->nRec = iFile;

                            pfi->FileAttr = pFile->deAttributes;
                            sec_temp = LE2BE(pFile->deCTime, 2);

                            sec_temp = LE2BE(pFile->deCDate, 2);


                            sec_temp = LE2BE(pFile->deMTime, 2);

                            sec_temp = LE2BE(pFile->deMDate, 2);


                            sec_temp = LE2BE(pFile->deADate, 2);
   

                            iFile = pArg->BytesPerSector;
                            iSec = sec_temp + pArg->SectorsPerClust;
                        }
                        else
                        {
                            iItem++;
                        }
                    }
                }
            }
        }
        while(!flag && (Cur_Clust = FAT32_GetNextCluster(Cur_Clust)) != 0x0fffffff);
        free(pFile);
        if(!flag)
        {
            return 1;
        }
        return 0;
    }
    else
    {
        return 2;
    }
    
}

/**************************************************************************
 - �����������ļ���λ
 - ����ģ�飺znFAT�ļ�ϵͳģ��
 - �������ԣ��ⲿ��ʹ�û�ʹ��
 - ����˵����pfi:FileInfoStruct���͵Ľṹ��ָ�룬����װ���ļ�������Ϣ���ļ�
             ��λ��pfi��ָ��Ľṹ���е���ز����ͱ������ˣ������ļ��ĵ�ǰ
             �������ļ���ǰ�����е�λ�ã��ļ��ĵ�ǰƫ�����ȵȣ��Ա�����ʹ��
             offset:Ҫ��λ��ƫ���������offset�����ļ��Ĵ�С����λ���ļ���
             ĩβ
 - ����˵�����ļ���λ�ɹ�����0������Ϊ1
 - ע���˺����������FAT32_Read_File���ã�����ʵ�ִ�ָ��λ�ö�ȡ���ݣ�������
       �û�ֱ�ӵ���Щ����
 **************************************************************************/

UINT8 FAT32_Seek_File(struct FileInfoStruct *pfi, UINT32 offset)
{
    UINT32 i, temp;

    if(offset <= pfi->FileSize)
    {
        if(offset == pfi->FileCurOffset)
        {
            pfi->FileCurPos %= pArg->BytesPerSector;
            return 1;
        }
        if(offset < pfi->FileCurOffset)
        {
            pfi->FileCurCluster = pfi->FileStartCluster;
            pfi->FileCurSector = SOC(pfi->FileCurCluster);
            pfi->FileCurPos = 0;
            pfi->FileCurOffset = 0;
        }
        if((offset - pfi->FileCurOffset) >= (pArg->BytesPerSector - pfi->FileCurPos))	 //Ŀ��ƫ�������ļ���ǰƫ����������ֽ�����С���ļ��ڵ�ǰ�����е�λ�õ�����ĩβ���ֽ���
        {
            pfi->FileCurOffset += (pArg->BytesPerSector - pfi->FileCurPos);
            pfi->FileCurPos = 0;
            if(pfi->FileCurSector - SOC(pfi->FileCurCluster) == (pArg->SectorsPerClust - 1))
            {
                pfi->FileCurCluster = FAT32_GetNextCluster(pfi->FileCurCluster);
                pfi->FileCurSector = SOC(pfi->FileCurCluster);
            }
            else
            {
                pfi->FileCurSector++;
            }
        }
        else
        {
            pfi->FileCurPos = (pfi->FileCurPos + offset - pfi->FileCurOffset) % pArg->BytesPerSector;
            pfi->FileCurOffset = offset;
            return 1;
        }
        temp = SOC(pfi->FileCurCluster);
        if((offset - (pfi->FileCurOffset)) / pArg->BytesPerSector + (pfi->FileCurSector - temp) > (pArg->SectorsPerClust - 1))
        {
            pfi->FileCurOffset += (((pArg->SectorsPerClust) - (pfi->FileCurSector - (SOC(pfi->FileCurCluster)))) * pArg->BytesPerSector);
            pfi->FileCurCluster = FAT32_GetNextCluster(pfi->FileCurCluster);
            pfi->FileCurSector = SOC(pfi->FileCurCluster);
            pfi->FileCurPos = 0;
        }
        else
        {
            pfi->FileCurSector += (offset - pfi->FileCurOffset) / pArg->BytesPerSector;
            pfi->FileCurPos = (offset - pfi->FileCurOffset) % pArg->BytesPerSector;
            pfi->FileCurOffset = offset;
            return 1;
        }

        temp = (offset - pfi->FileCurOffset) / (pArg->BytesPerSector * pArg->SectorsPerClust);
        for(i = 0; i < temp; i++)
        {
            pfi->FileCurCluster = FAT32_GetNextCluster(pfi->FileCurCluster);
        }
        pfi->FileCurOffset += (temp * (pArg->BytesPerSector * pArg->SectorsPerClust));
        pfi->FileCurSector = (SOC(pfi->FileCurCluster)) + (offset - pfi->FileCurOffset) / pArg->BytesPerSector;
        pfi->FileCurPos = ((offset - pfi->FileCurOffset)) % pArg->BytesPerSector;
        pfi->FileCurOffset = offset;
    }
    else
    {
        return 1;
    }
    return 0;
}

/**************************************************************************
 - �������������ļ���ĳһ��λ�ô�����ȡһ�����ȵ����ݣ��������ݻ�������
 - ����ģ�飺znFAT�ļ�ϵͳģ��
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
 - ע���ڶ�ȡһ���ļ�������ǰ�������Ƚ����ļ���FAT32_Open_File��
 **************************************************************************/

UINT32 FAT32_Read_File(struct FileInfoStruct *pfi, UINT32 offset, UINT32 len, UINT8 *pbuf)
{
    UINT32 i, j, k, temp;
    UINT32 counter = 0;
    if(offset < pfi->FileSize)
    {
        if(offset + len > pfi->FileSize) len = pfi->FileSize - offset;
        FAT32_Seek_File(pfi, offset);

        FAT32_ReadSector(pfi->FileCurSector, FAT32_Buffer);
        for(i = pfi->FileCurPos; i < pArg->BytesPerSector; i++)
        {
            if(counter >= len)
            {
                return len;
            }
            pbuf[counter] = FAT32_Buffer[i];
            counter++;
            pfi->FileCurPos++;
            pfi->FileCurOffset++;
        }
        if(pfi->FileCurSector - (SOC(pfi->FileCurCluster)) != (pArg->SectorsPerClust - 1))
        {
            for(j = pfi->FileCurSector + 1; j < (SOC(pfi->FileCurCluster)) + pArg->SectorsPerClust; j++)
            {
                FAT32_ReadSector(j, FAT32_Buffer);
                pfi->FileCurSector = j;
                for(i = 0; i < pArg->BytesPerSector; i++)
                {
                    if(counter >= len)
                    {
                        return len;
                    }
                    pbuf[counter] = FAT32_Buffer[i];
                    counter++;
                    pfi->FileCurPos++;
                    pfi->FileCurOffset++;
                }
            }
        }
        temp = (len - counter) / (pArg->BytesPerSector * pArg->SectorsPerClust);
        for(k = 0; k < temp; k++)
        {
            pfi->FileCurCluster = FAT32_GetNextCluster(pfi->FileCurCluster);
            for(j = (SOC(pfi->FileCurCluster)); j < (SOC(pfi->FileCurCluster)) + pArg->SectorsPerClust; j++)
            {
                FAT32_ReadSector(j, FAT32_Buffer);
                pfi->FileCurSector = j;
                for(i = 0; i < pArg->BytesPerSector; i++)
                {
                    if(counter >= len)
                    {
                        return len;
                    }
                    pbuf[counter] = FAT32_Buffer[i];
                    counter++;
                    pfi->FileCurOffset++;
                    pfi->FileCurPos++;
                    pfi->FileCurPos %= pArg->BytesPerSector;
                }
            }
        }
        pfi->FileCurCluster = FAT32_GetNextCluster(pfi->FileCurCluster);
        temp = (SOC(pfi->FileCurCluster)) + ((len - counter) / pArg->BytesPerSector);
        pfi->FileCurSector = (SOC(pfi->FileCurCluster));
        for(j = (SOC(pfi->FileCurCluster)); j < temp; j++)
        {
            FAT32_ReadSector(j, FAT32_Buffer);
            pfi->FileCurSector = j;
            for(i = 0; i < pArg->BytesPerSector; i++)
            {
                if(counter >= len)
                {
                    return len;
                }
                pbuf[counter] = FAT32_Buffer[i];
                counter++;
                pfi->FileCurPos++;
                pfi->FileCurPos %= pArg->BytesPerSector;
                pfi->FileCurOffset++;
            }
        }
        pfi->FileCurSector = j;
        FAT32_ReadSector(pfi->FileCurSector, FAT32_Buffer);
        temp = len - counter;
        for(i = 0; i < temp; i++)
        {
            if(counter >= len)
            {
                return len;
            }
            pbuf[counter] = FAT32_Buffer[i];
            counter++;
            pfi->FileCurPos++;
            pfi->FileCurPos %= pArg->BytesPerSector;
            pfi->FileCurOffset++;
        }
    }
    else
    {
        len = 0;
    }
    return len;
}

/**************************************************************************
 - �������������ļ�ĳһλ�ö�ȡһ�����ȵ����ݣ���pfun��ָ��ĺ���������
 - ����ģ�飺znFAT�ļ�ϵͳģ��
 - �������ԣ��ⲿ��ʹ�û�ʹ��
 - ����˵����pfi:FileInfoStruct���͵Ľṹ��ָ�룬����װ���ļ�������Ϣ���ļ�
             ��ȡ�Ĺ����У��˽ṹ���е���ز�������£������ļ��ĵ�ǰƫ������
             �ļ��ĵ�ǰ�������ļ��ĵ�ǰ�صȵ�
             offset:Ҫ��λ��ƫ������ҪС���ļ��Ĵ�С
             len:Ҫ��ȡ�����ݵĳ��ȣ����len+offset�����ļ��Ĵ�С����ʵ�ʶ�
             ȡ���������Ǵ�offset��ʼ���ļ�����
             pfun:�Զ�ȡ�����ݵĴ�������pfunָ��������������������
             �����������ȥ���������Ƿ��ڻ������У����ǰ�����ͨ�����ڷ���
             ��ȥ��ֻ��Ҫpfunȥָ����Ӧ�Ĵ�����������
 - ����˵������ȡ����ʵ�ʵ����ݳ��ȣ������ȡʧ�ܣ�����ָ����ƫ�����������ļ�
             ��С���򷵻�0
 - ע���ڶ�ȡһ���ļ�������ǰ�������Ƚ����ļ���FAT32_Open_File��
 **************************************************************************/

UINT32 FAT32_Read_FileX(struct FileInfoStruct *pfi, UINT32 offset, UINT32 len, void (*pfun)(UINT8))
{
    UINT32 i, j, k, temp;
    UINT32 counter = 0;
    if(offset < pfi->FileSize)
    {
        if(offset + len > pfi->FileSize) len = pfi->FileSize - offset;
        FAT32_Seek_File(pfi, offset);

        FAT32_ReadSector(pfi->FileCurSector, FAT32_Buffer);
        for(i = pfi->FileCurPos; i < pArg->BytesPerSector; i++)
        {
            if(counter >= len)
            {
                return len;
            }
            (*pfun)(FAT32_Buffer[i]);
            counter++;
            pfi->FileCurPos++;
            pfi->FileCurOffset++;
        }
        if(pfi->FileCurSector - (SOC(pfi->FileCurCluster)) != (pArg->SectorsPerClust - 1))
        {
            for(j = pfi->FileCurSector + 1; j < (SOC(pfi->FileCurCluster)) + pArg->SectorsPerClust; j++)
            {
                FAT32_ReadSector(j, FAT32_Buffer);
                pfi->FileCurSector = j;
                for(i = 0; i < pArg->BytesPerSector; i++)
                {
                    if(counter >= len)
                    {
                        return len;
                    }
                    (*pfun)(FAT32_Buffer[i]);
                    counter++;
                    pfi->FileCurPos++;
                    pfi->FileCurOffset++;
                }
            }
        }
        temp = (len - counter) / (pArg->BytesPerSector * pArg->SectorsPerClust);
        for(k = 0; k < temp; k++)
        {
            pfi->FileCurCluster = FAT32_GetNextCluster(pfi->FileCurCluster);
            for(j = (SOC(pfi->FileCurCluster)); j < (SOC(pfi->FileCurCluster)) + pArg->SectorsPerClust; j++)
            {
                FAT32_ReadSector(j, FAT32_Buffer);
                pfi->FileCurSector = j;
                for(i = 0; i < pArg->BytesPerSector; i++)
                {
                    if(counter >= len)
                    {
                        return len;
                    }
                    (*pfun)(FAT32_Buffer[i]);
                    counter++;
                    pfi->FileCurOffset++;
                    pfi->FileCurPos++;
                    pfi->FileCurPos %= pArg->BytesPerSector;
                }
            }
        }
        pfi->FileCurCluster = FAT32_GetNextCluster(pfi->FileCurCluster);
        temp = (SOC(pfi->FileCurCluster)) + ((len - counter) / pArg->BytesPerSector);
        pfi->FileCurSector = (SOC(pfi->FileCurCluster));
        for(j = (SOC(pfi->FileCurCluster)); j < temp; j++)
        {
            FAT32_ReadSector(j, FAT32_Buffer);
            pfi->FileCurSector = j;
            for(i = 0; i < pArg->BytesPerSector; i++)
            {
                if(counter >= len)
                {
                    return len;
                }
                (*pfun)(FAT32_Buffer[i]);
                counter++;
                pfi->FileCurPos++;
                pfi->FileCurPos %= pArg->BytesPerSector;
                pfi->FileCurOffset++;
            }
        }
        pfi->FileCurSector = j;
        FAT32_ReadSector(pfi->FileCurSector, FAT32_Buffer);
        temp = len - counter;
        for(i = 0; i < temp; i++)
        {
            if(counter >= len)
            {
                return len;
            }
            (*pfun)(FAT32_Buffer[i]);
            counter++;
            pfi->FileCurPos++;
            pfi->FileCurPos %= pArg->BytesPerSector;
            pfi->FileCurOffset++;
        }
    }
    else
    {
        len = 0;
    }
    return len;
}

/**************************************************************************
 - ����������Ѱ�ҿ��õĿ��д�
 - ����ģ�飺znFAT�ļ�ϵͳģ��
 - �������ԣ��ڲ�
 - ����˵������
 - ����˵��������ҵ��˿��дأ����ؿ��дصĴغţ����򷵻�0
 - ע��Ѱ�ҿ��д��Ǵ���Ŀ¼/�ļ��Լ����ļ�д�����ݵĻ�����������ܺܿ��Ѱ
       �ҵ����дأ���ô����Ŀ¼/�ļ��Լ����ļ�д��������Щ����Ҳ��ȽϿ졣
       �������Ǿ�������ʼ�Ĵ�����ȥѰ�ң�����ʹ���˶����������㷨���Դ�
       ���Ϻõ�Ч����������д�û���ҵ������п��ܾ�˵���洢�豸�Ѿ�û�пռ�
       ��
 **************************************************************************/

UINT32 FAT32_Find_Free_Clust(unsigned char flag)
{
    UINT32 iClu, iSec;
    struct FAT32_FAT *pFAT;
    for(iSec = pArg->FirstFATSector + temp_last_cluster / 128; iSec < pArg->FirstFATSector + pArg->FATsectors; iSec++)
    {
        FAT32_ReadSector(iSec, FAT32_Buffer);
        pFAT = (struct FAT32_FAT *)FAT32_Buffer;
        for(iClu = 0; iClu < pArg->BytesPerSector / 4; iClu++)
        {
            if(LE2BE((UINT8 *)(&((pFAT->Items))[iClu]), 4) == 0)
            {
                if(!flag)
                {
                    FAT32_Update_FSInfo_Free_Clu(0);
                    temp_last_cluster = 128 * (iSec - pArg->FirstFATSector) + iClu;
                    return temp_last_cluster;
                }
                else
                {
                    FAT32_Update_FSInfo_Last_Clu(128 * (iSec - pArg->FirstFATSector) + iClu);
                    return 128 * (iSec - pArg->FirstFATSector) + iClu;
                }
            }
        }
    }
    return 0;
}

/**************************************************************************
 - ��������������ļ�/Ŀ¼��
 - ����ģ�飺znFAT�ļ�ϵͳģ��
 - �������ԣ��ڲ�
 - ����˵����prec:ָ��һ��direntry���͵Ľṹ�壬���Ľṹ����FAT32���ļ�/
             Ŀ¼��Ľṹ
             name:�ļ���Ŀ¼������
             is_dir:ָʾ����ļ�/Ŀ¼�����ļ�����Ŀ¼���ֱ�����ʵ���ļ���
             Ŀ¼�Ĵ��� 1��ʾ����Ŀ¼ 0��ʾ�����ļ�
 - ����˵������
 - ע�����ﴴ���ļ���Ŀ¼�ķ����ǣ��Ƚ��ļ���Ŀ¼����Ϣ��䵽һ���ṹ���У�
       Ȼ���ٽ�����ṹ�������д�뵽�洢�豸����Ӧ����������Ӧλ����ȥ����
       ����������ļ���Ŀ¼�Ĵ�����
       ������ļ���Ŀ¼����Ϣʱ���ļ���Ŀ¼���״ز�û�����ȥ������ȫ0
 **************************************************************************/

void Fill_Rec_Inf(struct direntry *prec, INT8 *name, UINT8 is_dir, UINT8 *ptd)
{
    UINT8 i = 0, len = 0;
    UINT16 temp;

    if(is_dir)
    {
        len = strlen(name);
        if(len > 8)
        {
            for(i = 0; i < 6; i++)
            {
                (prec->deName)[i] = L2U(name[i]);
            }
            (prec->deName)[6] = '~';
            (prec->deName)[7] = '1';
        }
        else
        {
            for(i = 0; i < len; i++)
            {
                (prec->deName)[i] = L2U(name[i]);
            }
            for(; i < 8; i++)
            {
                (prec->deName)[i] = ' ';
            }
        }
        for(i = 0; i < 3; i++)
        {
            (prec->deExtension)[i] = ' ';
        }
    }
    else
    {
        while(name[len] != '.' && name[len] != 0) len++;
        if(len > 8)
        {
            for(i = 0; i < 6; i++)
            {
                (prec->deName)[i] = L2U(name[i]);
            }
            (prec->deName)[6] = '~';
            (prec->deName)[7] = '1';
        }
        else
        {
            for(i = 0; i < len; i++)
            {
                (prec->deName)[i] = L2U(name[i]);
            }
            for(; i < 8; i++)
            {
                (prec->deName)[i] = ' ';
            }
        }
        if(name[len] == 0)
        {
            for(i = 0; i < 3; i++)
            {
                (prec->deExtension)[i] = ' ';
            }
        }
        else
        {
            for(i = 0; i < 3; i++)
            {
                (prec->deExtension)[i] = ' ';
            }
            len++;
            i = 0;
            while(name[len] != 0)
            {
                (prec->deExtension)[i++] = L2U(name[len]);
                len++;
            }
        }
    }
    if(is_dir)
        (prec->deAttributes) = 0x10;
    else
        (prec->deAttributes) = 0x20;

    temp = MAKE_FILE_TIME(ptd[3], ptd[4], ptd[5]);
    (prec->deCTime)[0] = temp;
    (prec->deCTime)[1] = temp >> 8;
    temp = MAKE_FILE_DATE(ptd[0], ptd[1], ptd[2]);
    (prec->deCDate)[0] = temp;
    (prec->deCDate)[1] = temp >> 8;

    (prec->deLowerCase) = 0;
    (prec->deHighClust)[0] = 0;
    (prec->deHighClust)[1] = 0;
    (prec->deLowCluster)[0] = 0;
    (prec->deLowCluster)[1] = 0;
    for(i = 0; i < 4; i++)
    {
        (prec->deFileSize)[i] = 0;
    }
}

/**************************************************************************
 - ��������������FAT��
 - ����ģ�飺znFAT�ļ�ϵͳģ��
 - �������ԣ��ڲ�
 - ����˵����cluster:Ҫ���µĴ����
             dat:Ҫ����Ӧ�Ĵ������Ϊdat
 - ����˵������
 - ע�������ļ�д�������ݺ���Ҫ��FAT����и����Ա��������ݵĴ�����ϵ
       ɾ���ļ���ʱ��Ҳ��Ҫ�����ļ��Ĵ����������������ļ��Ĵ�����ϵ
 **************************************************************************/

void FAT32_Modify_FAT(UINT32 cluster, UINT32 dat)
{
    FAT32_ReadSector(pArg->FirstFATSector + (cluster * 4 / pArg->BytesPerSector), FAT32_Buffer);
    FAT32_Buffer[((cluster*4)%pArg->BytesPerSector)+0] = dat & 0x000000ff;
    FAT32_Buffer[((cluster*4)%pArg->BytesPerSector)+1] = (dat & 0x0000ff00) >> 8;
    FAT32_Buffer[((cluster*4)%pArg->BytesPerSector)+2] = (dat & 0x00ff0000) >> 16;
    FAT32_Buffer[((cluster*4)%pArg->BytesPerSector)+3] = (dat & 0xff000000) >> 24;
    FAT32_WriteSector(pArg->FirstFATSector + (cluster * 4 / pArg->BytesPerSector), FAT32_Buffer);

    FAT32_ReadSector(pArg->FirstFATSector + pArg->FATsectors + (cluster * 4 / pArg->BytesPerSector), FAT32_Buffer);
    FAT32_Buffer[((cluster*4)%pArg->BytesPerSector)+0] = dat & 0x000000ff;
    FAT32_Buffer[((cluster*4)%pArg->BytesPerSector)+1] = (dat & 0x0000ff00) >> 8;
    FAT32_Buffer[((cluster*4)%pArg->BytesPerSector)+2] = (dat & 0x00ff0000) >> 16;
    FAT32_Buffer[((cluster*4)%pArg->BytesPerSector)+3] = (dat & 0xff000000) >> 24;
    FAT32_WriteSector(pArg->FirstFATSector + pArg->FATsectors + (cluster * 4 / pArg->BytesPerSector), FAT32_Buffer);
}

/**************************************************************************
 - �������������ĳ���ص��������������0
 - ����ģ�飺znFAT�ļ�ϵͳģ��
 - �������ԣ��ڲ�
 - ����˵����cluster:Ҫ��յĴصĴغ�
 - ����˵������
 **************************************************************************/

void FAT32_Empty_Cluster(UINT32 Cluster)
{
    UINT32 iSec;
    UINT16 i;
    for(i = 0; i < pArg->BytesPerSector; i++)
    {
        FAT32_Buffer[i] = 0;
    }
    for(iSec = SOC(Cluster); iSec < SOC(Cluster) + pArg->SectorsPerClust; iSec++)
    {
        FAT32_WriteSector(iSec, FAT32_Buffer);
    }
}

/**************************************************************************
 - �����������ڴ洢�豸�д���һ���ļ�/Ŀ¼��
 - ����ģ�飺znFAT�ļ�ϵͳģ��
 - �������ԣ��ڲ�
 - ����˵����pfi:ָ��FileInfoStruct���͵Ľṹ�壬����װ�ظմ������ļ�����Ϣ
                 Ҳ����˵�������������Ŀ¼����˽ṹ�岻�ᱻ����
             cluster:��cluster������д����ļ�/Ŀ¼�����ʵ��������Ŀ¼��
                 �����ļ���Ŀ¼������ͨ��FAT32_Enter_Dir����ȡĳһ��Ŀ¼�Ŀ�
                 ʼ��
             name:�ļ�/Ŀ¼������
             is_dir:ָʾҪ���������ļ�����Ŀ¼���ļ���Ŀ¼�Ĵ��������ǲ�ͬ��
                 1��ʾ����Ŀ¼ 0��ʾ�����ļ�
 - ����˵�����ɹ�����1��ʧ�ܷ���-1
 **************************************************************************/

UINT8 FAT32_Create_Rec(struct FileInfoStruct *pfi, UINT32 cluster, INT8 *name, UINT8 is_dir, UINT8 *ptd)
{
    UINT32 iSec, iRec, temp_sec, temp_clu, new_clu, i, old_clu;
    UINT8 flag = 0;
    UINT16 temp_Rec;
    struct direntry *pRec;
    Fill_Rec_Inf(&temp_rec, name, is_dir, ptd);
    do
    {
        old_clu = cluster;
        temp_sec = SOC(cluster);
        for(iSec = temp_sec; iSec < temp_sec + pArg->SectorsPerClust; iSec++)
        {
            FAT32_ReadSector(iSec, FAT32_Buffer);
            for(iRec = 0; iRec < pArg->BytesPerSector; iRec += sizeof(struct direntry))
            {
                pRec = (struct direntry *)(FAT32_Buffer + iRec);
                if((pRec->deName)[0] == 0)
                {
                    flag = 1;
                    if(is_dir)
                    {
                        if(!(new_clu = FAT32_Find_Free_Clust(0))) return -1;
                        FAT32_Modify_FAT(new_clu, 0x0fffffff);
                        (temp_rec.deHighClust)[0] = (new_clu & 0x00ff0000) >> 16;
                        (temp_rec.deHighClust)[1] = (new_clu & 0xff000000) >> 24;
                        (temp_rec.deLowCluster)[0] = (new_clu & 0x000000ff);
                        (temp_rec.deLowCluster)[1] = (new_clu & 0x0000ff00) >> 8;
                    }
                    FAT32_ReadSector(iSec, FAT32_Buffer);
                    for(i = 0; i < sizeof(struct direntry); i++)
                    {
                        ((UINT8 *)pRec)[i] = ((UINT8 *)(&temp_rec))[i];
                    }
                    FAT32_WriteSector(iSec, FAT32_Buffer);
                    temp_sec = iSec;
                    temp_Rec = iRec;
                    iRec = pArg->BytesPerSector;
                    iSec = temp_sec + pArg->SectorsPerClust;
                }
            }
        }
    }
    while(!flag && (cluster = FAT32_GetNextCluster(cluster)) != 0x0fffffff);
    if(!flag)
    {
        if(!(temp_clu = FAT32_Find_Free_Clust(0))) return -1;
        FAT32_Modify_FAT(temp_clu, 0x0fffffff);
        FAT32_Modify_FAT(old_clu, temp_clu);
        temp_sec = SOC(temp_clu);
        temp_Rec = 0;
        FAT32_ReadSector(temp_sec, FAT32_Buffer);
        if(is_dir)
        {
            if(!(new_clu = FAT32_Find_Free_Clust(0))) return -1;
            FAT32_Modify_FAT(new_clu, 0x0fffffff);
            FAT32_ReadSector(temp_sec, FAT32_Buffer);
            (temp_rec.deHighClust)[0] = (new_clu & 0x00ff0000) >> 16;
            (temp_rec.deHighClust)[1] = (new_clu & 0xff000000) >> 24;
            (temp_rec.deLowCluster)[0] = (new_clu & 0x000000ff);
            (temp_rec.deLowCluster)[1] = (new_clu & 0x0000ff00) >> 8;
        }
        for(i = 0; i < sizeof(struct direntry); i++)
        {
            FAT32_Buffer[i] = ((UINT8 *)(&temp_rec))[i];
        }
        FAT32_WriteSector(temp_sec, FAT32_Buffer);
    }
    if(is_dir)
    {
        FAT32_Empty_Cluster(new_clu);

        Fill_Rec_Inf(&temp_rec, ".", 1, ptd);
        (temp_rec.deHighClust)[0] = (new_clu & 0x00ff0000) >> 16;
        (temp_rec.deHighClust)[1] = (new_clu & 0xff000000) >> 24;
        (temp_rec.deLowCluster)[0] = (new_clu & 0x000000ff);
        (temp_rec.deLowCluster)[1] = (new_clu & 0x0000ff00) >> 8;
        for(i = 0; i < sizeof(struct direntry); i++)
        {
            FAT32_Buffer[i] = ((UINT8 *)(&temp_rec))[i];
        }
        Fill_Rec_Inf(&temp_rec, "..", 1, ptd);
        if(cluster == pArg->FirstDirClust)
        {
            (temp_rec.deHighClust)[0] = 0;
            (temp_rec.deHighClust)[1] = 0;
            (temp_rec.deLowCluster)[0] = 0;
            (temp_rec.deLowCluster)[1] = 0;
        }
        else
        {
            (temp_rec.deHighClust)[0] = (cluster & 0x00ff0000) >> 16;
            (temp_rec.deHighClust)[1] = (cluster & 0xff000000) >> 24;
            (temp_rec.deLowCluster)[0] = (cluster & 0x000000ff);
            (temp_rec.deLowCluster)[1] = (cluster & 0x0000ff00) >> 8;
        }

        for(i = sizeof(struct direntry); i < 2 * sizeof(struct direntry); i++)
        {
            FAT32_Buffer[i] = ((UINT8 *)(&temp_rec))[i-sizeof(struct direntry)];
        }
        for(; i < pArg->BytesPerSector; i++)
        {
            FAT32_Buffer[i] = 0;
        }
        temp_sec = SOC(new_clu);
        FAT32_WriteSector(temp_sec, FAT32_Buffer);
    }
    else
    {
        strcpy(pfi->FileName, name);
        pfi->FileStartCluster = 0;
        pfi->FileCurCluster = 0;
        pfi->FileSize = 0;
        pfi->FileCurSector = 0;
        pfi->FileCurPos = 0;
        pfi->FileCurOffset = 0;
        pfi->Rec_Sec = temp_sec;
        pfi->nRec = temp_Rec;

        pfi->FileAttr = temp_rec.deAttributes;
    }
    FAT32_Find_Free_Clust(1);
    free(pRec);
    return 1;
}

/**************************************************************************
 - ������������ĳһ���ļ�׷������
 - ����ģ�飺znFAT�ļ�ϵͳģ��
 - �������ԣ��ⲿ��ʹ�û�ʹ��
 - ����˵����pfi:ָ��FileInfoStruct���͵Ľṹ�壬����װ�ظմ������ļ�����Ϣ
             len:Ҫ׷�ӵ����ݳ���
             pbuf:ָ�����ݻ�������ָ��
 - ����˵�����ɹ�����ʵ��д������ݳ��ȣ�ʧ�ܷ���0
 - ע��׷������ʧ�ܺ��п����Ǵ洢�豸�Ѿ�û�пռ��ˣ�Ҳ�����Ҳ������д���
 **************************************************************************/

UINT32 FAT32_Add_Dat(struct FileInfoStruct *pfi, UINT32 len, UINT8 *pbuf)
{
    UINT32 i = 0, counter = 0, iSec, iClu;
    UINT32 temp_sub, temp_file_size, new_clu, temp_sec;
    struct direntry *prec;
    if(len > 0)
    {
        FAT32_ReadSector(pfi->Rec_Sec, FAT32_Buffer);
        prec = (struct direntry *)(FAT32_Buffer + pfi->nRec);
        temp_file_size = LE2BE((prec->deFileSize), 4);
        if(!temp_file_size)
        {
            if(!(new_clu = FAT32_Find_Free_Clust(0))) return 0;
            FAT32_Modify_FAT(new_clu, 0x0fffffff);
            pfi->FileStartCluster = new_clu;
            pfi->FileCurCluster = pfi->FileStartCluster;
            pfi->FileSize = 0;
            pfi->FileCurSector = SOC(pfi->FileCurCluster);
            pfi->FileCurPos = 0;
            pfi->FileCurOffset = 0;
            FAT32_ReadSector(pfi->Rec_Sec, FAT32_Buffer);
            (prec->deHighClust)[0] = (new_clu & 0x00ff0000) >> 16;
            (prec->deHighClust)[1] = (new_clu & 0xff000000) >> 24;
            (prec->deLowCluster)[0] = (new_clu & 0x000000ff);
            (prec->deLowCluster)[1] = (new_clu & 0x0000ff00) >> 8;
            FAT32_WriteSector(pfi->Rec_Sec, FAT32_Buffer);
        }
        else
        {
            if(!(temp_file_size % (pArg->SectorsPerClust * pArg->BytesPerSector))) //�ڴص���ĩβ�ٽ�ط�����ҪѰ���´�
            {
                FAT32_Seek_File(pfi, pfi->FileSize - 1);
                if(!(new_clu = FAT32_Find_Free_Clust(0))) return 0;
                FAT32_Modify_FAT(pfi->FileCurCluster, new_clu);
                FAT32_Modify_FAT(new_clu, 0x0fffffff);
            }
            FAT32_Seek_File(pfi, pfi->FileSize);
        }

        iSec = pfi->FileCurSector;

        FAT32_ReadSector(iSec, FAT32_Buffer);
        for(i = pfi->FileCurPos; i < pArg->BytesPerSector; i++)
        {
            FAT32_Buffer[i] = pbuf[counter];
            counter++;
            if(counter >= len)
            {
                iSec = pfi->FileCurSector;
                goto end;
            }
        }
        FAT32_WriteSector(pfi->FileCurSector, FAT32_Buffer); //���ݽӷ�

        if(pfi->FileCurSector - (SOC(pfi->FileCurCluster)) < (pArg->SectorsPerClust - 1)) //�ж��ǲ���һ���ص����һ������,�Ƚ���ǰ��������������
        {
            for(iSec = pfi->FileCurSector + 1; iSec <= (SOC(pfi->FileCurCluster) + pArg->SectorsPerClust - 1); iSec++)
            {
                for(i = 0; i < pArg->BytesPerSector; i++)
                {
                    FAT32_Buffer[i] = pbuf[counter];
                    counter++;
                    if(counter >= len)
                    {
                        goto end;
                    }
                }
                FAT32_WriteSector(iSec, FAT32_Buffer);
            }
        }

        temp_sub = len - counter;
        for(iClu = 0; iClu < temp_sub / (pArg->SectorsPerClust * pArg->BytesPerSector); iClu++)
        {
            if(!(new_clu = FAT32_Find_Free_Clust(0))) return 0;
            FAT32_Modify_FAT(pfi->FileCurCluster, new_clu);
            FAT32_Modify_FAT(new_clu, 0x0fffffff);
            pfi->FileCurCluster = new_clu;

            temp_sec = SOC(new_clu);
            for(iSec = temp_sec; iSec < temp_sec + pArg->SectorsPerClust; iSec++)
            {
                for(i = 0; i < pArg->BytesPerSector; i++)
                {
                    FAT32_Buffer[i] = pbuf[counter];
                    counter++;
                }
                FAT32_WriteSector(iSec, FAT32_Buffer);
            }
        }

        temp_sub = len - counter;
        if(temp_sub)
        {
            if(!(new_clu = FAT32_Find_Free_Clust(0))) return 0;
            FAT32_Modify_FAT(pfi->FileCurCluster, new_clu);
            FAT32_Modify_FAT(new_clu, 0x0fffffff);
            pfi->FileCurCluster = new_clu;
            temp_sec = SOC(new_clu);
            for(iSec = temp_sec; iSec < temp_sec + temp_sub / pArg->BytesPerSector; iSec++)
            {
                for(i = 0; i < pArg->BytesPerSector; i++)
                {
                    FAT32_Buffer[i] = pbuf[counter];
                    counter++;
                }
                FAT32_WriteSector(iSec, FAT32_Buffer);
            }
        }

        temp_sub = len - counter;
        if(temp_sub)
        {
            for(i = 0; i < pArg->BytesPerSector; i++)
            {
                FAT32_Buffer[i] = pbuf[counter];
                counter++;
            }
            FAT32_WriteSector(iSec, FAT32_Buffer);
        }
end:
        FAT32_WriteSector(iSec, FAT32_Buffer);
        FAT32_ReadSector(pfi->Rec_Sec, FAT32_Buffer);
        (((struct direntry *)(FAT32_Buffer + pfi->nRec))->deFileSize)[0] = ((temp_file_size + len) & 0x000000ff);
        (((struct direntry *)(FAT32_Buffer + pfi->nRec))->deFileSize)[1] = ((temp_file_size + len) & 0x0000ff00) >> 8;
        (((struct direntry *)(FAT32_Buffer + pfi->nRec))->deFileSize)[2] = ((temp_file_size + len) & 0x00ff0000) >> 16;
        (((struct direntry *)(FAT32_Buffer + pfi->nRec))->deFileSize)[3] = ((temp_file_size + len) & 0xff000000) >> 24;
        FAT32_WriteSector(pfi->Rec_Sec, FAT32_Buffer);

        pfi->FileSize = (temp_file_size + len);
        pfi->FileCurSector = (pfi->FileSize % pArg->BytesPerSector) ? iSec : iSec + 1;
        pfi->FileCurPos = pfi->FileSize % pArg->BytesPerSector;
        pfi->FileCurOffset = pfi->FileSize;
    }
    FAT32_Find_Free_Clust(1);
    free(prec);
    return len;
}

/**************************************************************************
 - ��������������Ŀ¼(֧�������Ŀ¼����)
 - ����ģ�飺znFAT�ļ�ϵͳģ��
 - �������ԣ��ⲿ��ʹ�û�ʹ��
 - ����˵����pfi:����
             dirpath:Ŀ¼·�� ���� "\\dir1\\dir2\\dir3\\....\\dirn\\"
             ��������\\����
 - ����˵�����ɹ�����0��ʧ�ܷ���1
 - ע������м�ĳһ��Ŀ¼�����ڣ�������������·����dir3�����ڣ���ô�˺�����
       �������Ŀ¼��Ȼ���ټ���ȥ����������Ŀ¼
       ����Ŀ¼ʧ���п�������Ϊ�洢�豸�ռ䲻��
 **************************************************************************/

UINT8 FAT32_Create_Dir(struct FileInfoStruct *pfi, INT8 *dirpath, UINT8 *ptd)
{
    while(!FAT32_Enter_Dir(dirpath))
    {
        if(FAT32_Create_Rec(pfi, temp_dir_cluster, temp_dir_name, 1, ptd) == -1)
        {
            return 1;
        }
    }
    return 0;
}

/**************************************************************************
 - ���������������ļ�(֧�������Ŀ¼����)
 - ����ģ�飺znFAT�ļ�ϵͳģ��
 - �������ԣ��ⲿ��ʹ�û�ʹ��
 - ����˵����pfi:һ��ָ��FileInfoStruct���͵Ľṹ���ָ�룬����װ���´�����
             �ļ���Ϣ������´������ļ������ٴ򿪾Ϳ���ֱ��������
             filepath:�ļ�·�� ���� "\\dir1\\dir2\\dir3\\....\\dirn\\test.txt"
 - ����˵����0���ɹ� 1���ļ��Ѵ��� 2�������ļ�Ŀ¼ʧ�� 3�������ļ�ʧ��
 - ע������ļ�·����ĳһ���м�Ŀ¼�����ڣ���ô�˺����ᴴ�����Ŀ¼���ټ���
       ȥ����������Ŀ¼��һֱ�������ļ�������ɡ�
       �����ļ�ʧ���п�������Ϊ�洢�豸�ռ䲻�㣬���Ǵ��ļ��Ѿ�����
 **************************************************************************/

UINT8 FAT32_Create_File(struct FileInfoStruct *pfi, INT8 *filepath, UINT8 *ptd)
{
    if(FAT32_Open_File(pfi, filepath, 0))
    {
        if(!FAT32_Create_Dir(pfi, filepath, ptd))
        {
            if(FAT32_Create_Rec(pfi, temp_dir_cluster, temp_dir_name, 0, ptd) == -1)
            {
                return 3;
            }
        }
        else
        {
            return 2;
        }
    }
    else
    {
        return 1;
    }
    return 0;
}

/**************************************************************************
 - ����������ɾ���ļ�(֧�������Ŀ¼)
 - ����ģ�飺znFAT�ļ�ϵͳģ��
 - �������ԣ��ⲿ��ʹ�û�ʹ��
 - ����˵����filepath:�ļ�·�� ���� "\\dir1\\dir2\\dir3\\....\\dirn\\test.txt"
 - ����˵����1:�ļ���Ŀ¼·�������� 0:�ɹ�
 - ע��ɾ������ļ���FAT���еĴ�����ϵ��ȫ���ƻ�
 **************************************************************************/

UINT8 FAT32_Del_File(INT8 *filepath)
{
    UINT32 cur_clu, next_clu;
    if(FAT32_Open_File(&fi, filepath, 0))
    {
        return 1;
    }
    FAT32_ReadSector(fi.Rec_Sec, FAT32_Buffer);
    *(FAT32_Buffer + fi.nRec) = 0xe5;
    FAT32_WriteSector(fi.Rec_Sec, FAT32_Buffer);

    if(cur_clu = fi.FileStartCluster)
    {
        if(cur_clu < Search_Last_Usable_Cluster())
            FAT32_Update_FSInfo_Last_Clu(cur_clu);
        FAT32_Update_FSInfo_Free_Clu(1);
        next_clu = FAT32_GetNextCluster(cur_clu);
        while(next_clu != 0x0fffffff)
        {
            FAT32_Update_FSInfo_Free_Clu(1);
            FAT32_Modify_FAT(cur_clu, 0x00000000);
            cur_clu = next_clu;
            next_clu = FAT32_GetNextCluster(cur_clu);
        }
        FAT32_Modify_FAT(cur_clu, 0x00000000);
    }
    return 0;
}

/**************************************************************************
 - �����������ļ�����(Դ�ļ�·����Ŀ���ļ�·����֧���������Ŀ¼������֧��
             �ļ���ͨ��)
 - ����ģ�飺znFAT�ļ�ϵͳģ��
 - �������ԣ��ⲿ��ʹ�û�ʹ��
 - ����˵����pArg1:��Դ�ļ����ڵĴ洢�豸�ĳ�ʼ�����ṹ���ָ��
             pArg2:��Ŀ���ļ����ڵĴ洢�豸�ĳ�ʼ�����ṹ���ָ��
             sfilename:Դ�ļ�·����Ҳ���ǿ�������������Դ
             tfilename:Ŀ���ļ�·����Ҳ�����������յ�д����ļ�
                       ���� "\\dir1\\dir2\\dir3\\....\\dirn\\test.txt"
             file_buf:����������Ҫ�õ������ݻ��������˻���������Խ��
                      �����ٶ�Խ��
             buf_size:���ݻ������Ĵ�С
 - ����˵����1:Ŀ¼�ļ�����ʧ�� 2:Դ�ļ��򿪴�� 0:�ɹ�
 - ע���˺���֧�ֶ��豸֮����ļ�������pArg1��pArg2������Դ�洢�豸��Ŀ��
       �洢�豸�ĳ�ʼ������Ϣ���Ӷ�����ͬʱ�������洢�豸���в�����
	   znFAT 5.01�濪ʼ֧�ֶ��豸�����豸����໥���ݿ���������͵�Ӧ��
 **************************************************************************/

unsigned char FAT32_XCopy_File(struct FAT32_Init_Arg *pArg1, struct FAT32_Init_Arg *pArg2, INT8 *sfilename, INT8 *tfilename, UINT8 *file_buf, UINT32 buf_size, unsigned char *pt)
{
    struct FileInfoStruct FileInfo2, FileInfo1;
    UINT32 i;

    Dev_No = pArg2->DEV_No;
    pArg = pArg2;
    if(FAT32_Create_File(&FileInfo1, tfilename, pt)) return 1;
    Dev_No = pArg1->DEV_No;
    pArg = pArg1;
    if(FAT32_Open_File(&FileInfo2, sfilename, 0)) return 2;

    for(i = 0; i < FileInfo2.FileSize / buf_size; i++)
    {
        Dev_No = pArg1->DEV_No;
        pArg = pArg1;
        FAT32_Read_File(&FileInfo2, i * buf_size, buf_size, file_buf);
        Dev_No = pArg2->DEV_No;
        pArg = pArg2;
        FAT32_Add_Dat(&FileInfo1, buf_size, file_buf);
    }

    Dev_No = pArg1->DEV_No;
    pArg = pArg1;
    FAT32_Read_File(&FileInfo2, i * buf_size, FileInfo2.FileSize % buf_size, file_buf);
    Dev_No = pArg2->DEV_No;
    pArg = pArg2;
    FAT32_Add_Dat(&FileInfo1, FileInfo2.FileSize % buf_size, file_buf);

    return 0;
}

/**************************************************************************
 - �����������ļ�������
 - ����ģ�飺znFAT�ļ�ϵͳģ��
 - �������ԣ��ⲿ��ʹ�û�ʹ��
 - ����˵����filename:��Ҫ��������Դ�ļ���·�� ��\a.txt
             newfilename:Ŀ���ļ��� ��b.txt (עĿ���ļ����ǵ������ļ�����
			 ����·��)
 - ����˵����1:Դ�ļ��򿪴�� 0:�ɹ�
 - ע����
 **************************************************************************/

UINT8 FAT32_Rename_File(INT8 *filename, INT8 *newfilename)
{
    struct FileInfoStruct fi;
    UINT8 i = 0, j = 0;
    if(FAT32_Open_File(&fi, filename, 0)) return 1; //�ļ���ʧ��
    FAT32_ReadSector(fi.Rec_Sec, FAT32_Buffer);
    for(i = 0; i < 11; i++) (FAT32_Buffer + fi.nRec)[i] = 0x20;
    i = 0;
    while(newfilename[i] != '.')
    {
        (FAT32_Buffer + fi.nRec)[i] = L2U(newfilename[i]);
        i++;
    }
    i++;
    while(newfilename[i])
    {
        (FAT32_Buffer + fi.nRec + 8)[j] = L2U(newfilename[i]);
        i++;
        j++;
    }
    FAT32_WriteSector(fi.Rec_Sec, FAT32_Buffer);
    return 0;
}

/**************************************************************************
 - �����������ļ��ر�
 - ����ģ�飺znFAT�ļ�ϵͳģ��
 - �������ԣ��ⲿ��ʹ�û�ʹ��
 - ����˵����pfi:ָ��ǰ�򿪵��ļ����ļ���Ϣ�ṹ
 - ����˵����0:�ɹ�
 - ע����
 **************************************************************************/

UINT8 FAT32_File_Close(struct FileInfoStruct *pfi)
{
    UINT16 i = 0;
    for(i = 0; i < sizeof(struct FileInfoStruct); i++)
    {
        ((UINT8 *)pfi)[i] = 0;
    }
    return 0;
}


UINT8 FAT32_FINDFILES(INT8 *filepath,char filenames[][17],int max)
{
    UINT32 Cur_Clust, sec_temp, iSec, iFile, iItem = 0;
    UINT8 flag = 0, index = 0, i = 0;
    struct direntry *pFile;
    while(filepath[i] != 0)
    {
        if(filepath[i] == '\\') index = i;
        i++;
    }
    if(Cur_Clust = FAT32_Enter_Dir(filepath))
    {
        Str2Up(temp_dir_name);
        do
        {
            sec_temp = SOC(Cur_Clust);
            for(iSec = sec_temp; iSec < sec_temp + pArg->SectorsPerClust; iSec++)
            {
                FAT32_ReadSector(iSec, FAT32_Buffer);
                for(iFile = 0; iFile < pArg->BytesPerSector; iFile += sizeof(struct direntry))
                {
                    pFile = ((struct direntry *)(FAT32_Buffer + iFile));
                    FAT32_toFileName(pFile->deName, temp_file_name);
                    if(pFile->deName[0] != 0xe5 && pFile->deAttributes & 0x20) 
                    {
						strcpy(filenames[iItem++], temp_file_name);
                    }
                    if(iItem>=max) return max;
					free(pFile);
				}
            }
        }while(!flag && (Cur_Clust = FAT32_GetNextCluster(Cur_Clust)) != 0x0fffffff);
		return iItem;
    }
    else
    {
        return -1;
    }
    
}

