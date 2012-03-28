#include "menu.h"

int CursorLine=0;
int CheckValue(char * str)
{
	int i,l,count;
	l=strlen(str);
	count=0;
	if(str[0]=='.' ||str[l-1]=='.') return 0;
	for(i=0;i<l;i++)
	{
		if(str[i]=='.') count++;
	}
	if(count>1) return 0;
	return 1;
		
}
char* ValueEdit(char *src)
{
	char str[17];
	strcpy(str,src); 	
	for(;;)
	{	
		ClearScreen();	
		PrintLCD("Origin: ",0,0);
		PrintLCD(str,1,0);
		strcpy(src,str);
		PrintLCD("Modify to: ",2,0);				
		if(CheckValue(TextInput(3,0,src))) return src;
		ClearScreen();
		PrintLCD("ERROR",1,0);
		Delay_ms(2000);
	}
			
}

char* TextInput(int xPos,const int yPos,char *d)
{
	int i=0;
	int y=yPos;
	i=strlen(d);
	y=i;
	PrintLCD(d,xPos,yPos);
	for(;;)
	{
		switch(keyValue)
		{
			case Backspace:
			{
				keyValue=Null;
				
				if(i>0) 
				{
					if(i%2==0) 
					{
						i-=2;
						
					}
					else i--;
				}
				if(y>yPos) 
				{
					if(y%2==0) y-=2;
					else y--;
				}				
				SetDisplayPosition(xPos,y/2);
				LCD_WriteData(' ');
				LCD_WriteData(' ');
				SetDisplayPosition(xPos,y/2);
				break;
			}
			case Enter:
			{
				keyValue=Null;
				d[i]='\0';
				return d;
			}
			case num_0:
			{
				keyValue=Null;
				if(i>=16) break;
				d[i++]='0';
				y++;
				LCD_WriteData('0');
				break;
			}
			
			case num_1:
			{
				keyValue=Null;
				if(i>=16) break;
				d[i++]='1';
				y++;
				LCD_WriteData('1');
				break;
			}
			case num_2:
			{
				keyValue=Null;
				if(i>=16) break;
				d[i++]='2';
				y++;
				LCD_WriteData('2');
				break;
			}
			case num_3:
			{
				keyValue=Null;
				if(i>=16) break;
				d[i++]='3';
				y++;
				LCD_WriteData('3');
				break;
			}
			case num_4:
			{
				keyValue=Null;
				if(i>=16) break;
				d[i++]='4';
				y++;
				LCD_WriteData('4');
				break;
			}
			case num_5:
			{
				keyValue=Null;
				if(i>=16) break;
				d[i++]='5';
				y++;
				LCD_WriteData('5');
				break;
			}
			case num_6:
			{
				keyValue=Null;
				if(i>=16) break;
				d[i++]='6';
				y++;
				LCD_WriteData('6');
				break;
			}
			case num_7:
			{
				keyValue=Null;
				if(i>=16) break;
				d[i++]='7';
				y++;
				LCD_WriteData('7');
				break;
			}
			case num_8:
			{
				keyValue=Null;
				if(i>=16) break;
				d[i++]='8';
				y++;
				LCD_WriteData('8');
				break;
			}
			case num_9:
			{
				keyValue=Null;
				if(i>=16) break;
				d[i++]='9';
				y++;
				LCD_WriteData('9');
				break;
			}
			case Point:
			{
				keyValue=Null;
				if(i>=16) break;
				d[i++]='.';
				y++;
				LCD_WriteData('.');
				break;
			}
			case Multiply:
			{
				keyValue=Null;
				d[i]='\0';
				if(CheckValue(d)==0) break;
				Calculate(d,Multiply);
				PrintLCD(d,3,0);
				i=strlen(d);
				y=i;
			    break;
			}
			case Devide:
			{
				keyValue=Null;
				d[i]='\0';
				if(CheckValue(d)==0) break;
				Calculate(d,Devide);
				PrintLCD(d,3,0);
				i=strlen(d);
				y=i;
				break;	
			}
			case Plus:
			{
				keyValue=Null;
				d[i]='\0';
				if(CheckValue(d)==0) break;
				Calculate(d,Plus);
				PrintLCD(d,3,0);
				i=strlen(d);
				y=i;
				break;
			}
			case Minus:
			{
				keyValue=Null;
				d[i]='\0';
				if(CheckValue(d)==0) break;
				Calculate(d,Minus);
				PrintLCD(d,3,0);
				i=strlen(d);
				y=i;
				break;	
			}
		}
	}
	
}
int ShowMessage(char msg1[17],char msg2[17],int delaytime)
{
	ClearScreen();
	PrintLCD(msg1,0,0);
	PrintLCD(msg2,1,0);
	if(delaytime!=0)
	{
		Delay_ms(delaytime);
	} else
	{
		
		for(;;){
			switch(keyValue)
			{
				case Enter:
				{
					keyValue=Null;
					return 1;
				}
				case Backspace:
				{
					keyValue=Null;
					return 0;
				}
			}
		}
	}
	
}
