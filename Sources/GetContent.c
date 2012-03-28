#include "headers.h"

byte RXBUF[512] = {0};
FAT32_Init_Arg  Init_Arg_SD;	   //初始化参数结构体实体
FAT32_Init_Arg *pArg;

#define PLUS_ADD 0x0004
#define MINUS_ADD 0x0008
#define MUL_ADD 0x0012
#define DIV_ADD 0x0016
#define BACKUP_ADD 0x0000


char buf[40];
FileInfoStruct file;

void getConstData(float *PLUS,float *MINUS,float *MUL,float *DIV)
{
	
    word PLUSl,MINUSl,MULl,DIVl;
    word PLUSh,MINUSh,MULh,DIVh;
    DisableInterrupts;
    DFlash_Init();
    PLUSl=DFlash_Read(PLUS_ADD);
    PLUSh=DFlash_Read(PLUS_ADD+2);
    MINUSl=DFlash_Read(MINUS_ADD);
    MINUSh=DFlash_Read(MINUS_ADD+2);
    MULl=DFlash_Read(MUL_ADD);
    MULh=DFlash_Read(MUL_ADD+2);
    DIVl=DFlash_Read(DIV_ADD);
    DIVh=DFlash_Read(DIV_ADD+2);
    i16tof32(PLUS,PLUSh,PLUSl);
    i16tof32(MINUS,MINUSh,MINUSl);
    i16tof32(MUL,MULh,MULl);
    i16tof32(DIV,DIVh,DIVl);
    EnableInterrupts;
}
void setConstData(float PLUS,float MINUS,float MUL,float DIV)
{
	
    
	word PLUSl,MINUSl,MULl,DIVl;
    word PLUSh,MINUSh,MULh,DIVh;
    word backupcount;
    DisableInterrupts;
    f32toi16(PLUS,&PLUSh,&PLUSl);
    f32toi16(MINUS,&MINUSh,&MINUSl);
    f32toi16(MUL,&MULh,&MULl);
    f32toi16(DIV,&DIVh,&DIVl);
	DFlash_Init();
    DFlash_Erase(BACKUP_ADD);     //确保先擦除后写入
    DFlash_Write(backupcount,BACKUP_ADD,0);
    DFlash_Write(PLUS_ADD,PLUSl,PLUSh);
   	DFlash_Write(MINUS_ADD,MINUSl,MINUSh);
   	DFlash_Write(MUL_ADD,MULl,MULh);
	DFlash_Write(DIV_ADD,DIVl,DIVh);
	EnableInterrupts;
}
int initSDFAT()
{
    
    int i = 0;
    Dev_No = 0;
    Init_Dev();
    SD_Init();            //初始化spi口
    while(SD_Reset())     //复位是否成功
    {
       Delay_ms(10);
        i++;
        if(i > 5) return 0;
    }
    SPI_High_Rate();   	 //SPI进入高速操作模式
    pArg = &Init_Arg_SD;
    FAT32_Init();
    return 1;

}
int getString(FileInfoStruct *file, UINT32 offset, INT8 *str)
{
    INT8 c;
    int i = 0;

    for(; offset < file->FileSize; offset++)
    {
        ReadOneByteFromFile(file, offset, &c);
        if(c >= '0' && c <= '9' ||
                c >= 'a' && c <= 'z' ||
                c >= 'A' && c <= 'Z' ||
                c == '_' || c == '.' || c=='-')
            break;
    }
    for(i=0; offset < file->FileSize; offset++)
    {
        ReadOneByteFromFile(file, offset, &c);
        if(
            !(c >= '0' && c <= '9' ||
              c >= 'a' && c <= 'z' ||
              c >= 'A' && c <= 'Z' ||
              c == '_' || c == '.'||c=='-')
        )
            break;
        str[i++] = c;
    }
    str[i] = '\0';
    return offset;
}
int getParameter(char *filePath, char name[][17], char value[][17])
{
    int i = 0;
    int offset=0;
    int n;    
    INT8 buf[10];
    if(! initSDFAT() )
	{
		ShowMessage("SD卡初始化失败","请换卡或重插",1000);
    	return -1;		
	}	
    if( Open(&file, filePath, READONLY) )
    {
    	ShowMessage("文件打开失败","请确认文件是否存在",1000);
    	return -2;	
    }
    ReadBytesFromFile(&file, 0, 2, buf);
    if(buf[1] >= '0' && buf[1] <= '9')
    {
        n = (buf[0] - '0') * 10 + buf[1] - '0';
        offset = 2;
    }
    else
    {
        n = buf[0] - '0';
        offset = 1;
    }
    for(i = 0; i < n; i++)
    {
        offset = getString(&file, offset, name[i]);
        offset = getString(&file, offset, value[i]);
        
    }
    return n;
}

int setParameter(char *filePath, INT8 name[][17], INT8 value[][17], int n)
{
    int i;
    int offset = 0;
    char CL[2];
    CL[0]=0x0d;
    CL[1]=0x0a;
    if(!initSDFAT())
	{
		ShowMessage("SD卡初始化失败","请换卡或重插",1000);
    	return -1;		
	}
    if(Open(&file, filePath, WRITE))
    {
    	ShowMessage("文件创建失败","请确认是否有空间",1000);
    	return -2;	
    }   
    if(n < 10)
    {
        INT8 c = n + '0';
        WriteOneByteToFile(&file, &c);
    }
    else
    {
        INT8 c1 = n / 10+'0';
        INT8 c2 = n % 10+'0';
        WriteOneByteToFile(&file, &c1);
        WriteOneByteToFile(&file, &c2);
    }
    WriteBytesToFile(&file, 2, CL);
    for(i = 0; i < n; i++)
    {
        int l1 = strlen(name[i]);
        int l2 = strlen(value[i]);
        strcpy(buf,name[i]);
        strcat(buf," ");
        strcat(buf,value[i]);
        strcat(buf,CL);
        WriteBytesToFile(&file,l1+l2+3,buf);
    }
    return 1;

}
uint16 getBackupFiles(char name[][17])
{
	initSDFAT();
	return FAT32_FINDFILES("\\backup\\",name,MAX_BACKUPNUM);
}
uint16 getLogFiles(char name[][17])
{
	initSDFAT();
	return FAT32_FINDFILES("\\log\\",name,MAX_LOGNUM);
}
