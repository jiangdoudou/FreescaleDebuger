#include "menu.h"
#define READY 0xEC
#define BEGIN 0xED
#define PAUSE 0xEE
#define EOF 0xEF
#define DBBUG_LOG_PATH "\\log\\"

extern volatile char names[50][17];
extern volatile char values[50][17];
extern char backnames[50][17];
extern FileInfoStruct file;

char buffer[100];

char ComStr[2][17]={"发送参数","接收日记"};
void SendPara();
void ReceiveLog();
void ComMenu()
{
    DisplayInfo *comMenu=(DisplayInfo *)malloc(sizeof(DisplayInfo));
    comMenu->curPos = 0;
    comMenu->size = 2;
    init2DArray(&(comMenu->words), ComStr, comMenu->size);
    InitDisplay(comMenu);
    for(;;)
    {
        switch(keyValue)
        {
        case Up:
        {
            keyValue = Null;
            CursorMoveUp(comMenu);
            break;
        }
        case Down:
        {
            keyValue = Null;
            CursorMoveDown(comMenu);
            break;
        }
        case Backspace:
        {
        	keyValue = Null;
			free2DArray(&(comMenu->words),2); 
			free(comMenu);
            return;
        }	
        case Enter:
        {
        	int choice = comMenu->curPos; 
        	keyValue = Null;
                   
            switch(choice)
            {
            	case 0:SendPara();break;
            	case 1:ReceiveLog();break;
            }
            ResumeDisplay(comMenu);
            break;
        }
        default:
        {
            break;
        }
        }
    }
    free2DArray(&(comMenu->words),comMenu->size);
	free(comMenu);
}

void SendPara()
{
	char paraNumbers = (char)getParameter(FILE_PATH, names, values);
	int i=0;
	int flag=1;
	int r;
	DisableInterrupts;
	
	MCUInit(FBUS_32M);
	SCIInit(0,FBUS_32M,9600);
	SCISend1(0,BEGIN);
	SCISend1(0,paraNumbers);
    SCISend1(0,' ');    
    for(i = 0; i < (int)paraNumbers; i++)
    {
        int l1 = strlen(names[i]);
        int l2 = strlen(values[i]);
        
        SCISendN(0,l1,names[i]);
        SCISend1(0,' ');
        SCISendN(0,l2,values[i]);
        SCISend1(0,' ');       
    }
    
    SCISend1(0,EOF);
    ShowMessage("发送参数成功"," ",1000); 
    EnableInterrupts;		
}

void ReceiveLog()//互锁
{
	
	uint8 r,flag;
	int i=0;
	DisableInterrupts;
	ShowMessage("准备中，请稍候"," ",1);
	i=getLogFiles(backnames);
	if(i==0)
	{
		strcpy(buffer,"\\log\\0001.csv");
	}else
	{
		char temp[15];
		getMaxString(backnames,temp,i);
		dropFilenamePost(temp);
		FilenameInc(temp);
		strcpy(buffer,"\\log\\");
		strcat(buffer,temp);
		strcat(buffer,".csv");
	}
	
	if(!initSDFAT())
	{
		ShowMessage("SD卡初始化失败","请换卡或重插",1000);
    	return;		
	}
    if(Open(&file, buffer, WRITE))
    {
    	ShowMessage("文件创建失败","请确认是否有空间",1000);
    	return;	
    }
    ShowMessage("就绪"," ",1);	
	MCUInit(FBUS_32M);
	SCIInit(0,FBUS_32M,9600);
	SCISend1(0,READY);
	r=SCIRe1(0,&flag);
	while(flag==1 || r!=BEGIN)
	{
		r=SCIRe1(0,&flag);		
	}
	i=0;
	while(r!=EOF)
	{
		r=SCIRe1(0,&flag);
		if(flag==0 && r==PAUSE)
		{
			WriteBytesToFile(&file,i,buffer);
			i=0;
			MCUInit(FBUS_32M);
			SCIInit(0,FBUS_32M,19200);
			SCISend1(0,READY);
				
		} 
		else if(flag==0 && r==EOF)
		{
		 	WriteBytesToFile(&file,i,buffer);
		 	break;	
		}
		else  if(flag==0)
		{
			
			buffer[i++]=r;
		}
	}
	ShowMessage("接收成功"," ",1000);
		
	EnableInterrupts;			
}