#include "headers.h"


#define DDR_INI()  DDRS |= 0xE0
#define SD_Sel()   PTS_PTS7=0        //低电平
#define SD_deSel() PTS_PTS7=1        //高电平

extern byte RXBUF[];

#pragma CODE_SEG DEFAULT

void New_Buf8(byte buffer[], byte ch)
{
    //-- Tx & Rx Buffer initialize
    int i, j = 0;
    for(i = 0; i < 512; i++)
        *(buffer + i) = ch;
}

void CLS_Buf8(byte buffer[])
{
    //-- Flushing Rx buffer
    int i;
    for(i = 0; i < 512; i++)
        *(buffer + i) = 0;
}
void View_Buf8(byte buffer[])
{
    int i = 0;
    for(i = 0; i < 512; i++)
    {
        if((i % 32) == 0)	  printp("\n");
        printp("%02x,", buffer[i]);
    }
}

/*******************************************
*                初始化部分                *
********************************************/
void SCI_Init(void)
{
    SCI0CR2 = 0x2c; //enable Receive Full Interrupt,RX enable,Tx enable
    SCI0BDH = 0x00; //busclk  8MHz,19200bps,SCI0BDL=0x1a
    SCI0BDL = 0x34; //SCI0BDL=busclk/(16*SCI0BDL)=19200
}
void InitPorts(void)
{
    DDRP = 0xff;         // set  port B bit0 as output
    PTP = 0X0;
}
void SetBusCLK_16M(void)
{
    CLKSEL = 0X00;				// disengage PLL to system
    PLLCTL_PLLON = 1;			// turn on PLL
    SYNR = 0x00 | 0x01; 	// VCOFRQ[7:6];SYNDIV[5:0]
    // fVCO= 2*fOSC*(SYNDIV + 1)/(REFDIV + 1)
    // fPLL= fVCO/(2 × POSTDIV)
    // fBUS= fPLL/2
    // VCOCLK Frequency Ranges  VCOFRQ[7:6]
    // 32MHz <= fVCO <= 48MHz    00
    // 48MHz <  fVCO <= 80MHz    01
    // Reserved                  10
    // 80MHz <  fVCO <= 120MHz   11
    REFDV = 0x80 | 0x01; // REFFRQ[7:6];REFDIV[5:0]
    // fREF=fOSC/(REFDIV + 1)
    // REFCLK Frequency Ranges  REFFRQ[7:6]
    // 1MHz <= fREF <=  2MHz       00
    // 2MHz <  fREF <=  6MHz       01
    // 6MHz <  fREF <= 12MHz       10
    // fREF >  12MHz               11
    // pllclock=2*osc*(1+SYNR)/(1+REFDV)=32MHz;
    POSTDIV = 0x00;     // 4:0, fPLL= fVCO/(2xPOSTDIV)
    // If POSTDIV = $00 then fPLL is identical to fVCO (divide by one).
    _asm(nop);          // BUS CLOCK=16M
    _asm(nop);
    while(!(CRGFLG_LOCK == 1));	 //when pll is steady ,then use it;
    CLKSEL_PLLSEL = 1;		       //engage PLL to system;
}
void Init_Dev(void)
{
    SetBusCLK_16M();
    InitPorts();
    SCI_Init();
}

//写、读一个字节
byte SPI_RdWr_Byte(byte val)
{
    while (!SPI0SR_SPTEF);
    SPI0DRL = val;
    while(!(SPI0SR_SPIF)) ;
    return SPI0DRL;
}
void SPI_High_Rate(void)
{
    //最高操作速率不能高于25Mbps
    SPI0BR  = 0x01; //BR=busclk/((SPPR + 1)· 2^(SPR + 1))=32M/4=8M
};
//sd卡初始化
void SD_Init(void)
{
    MODRR_MODRR4 = 0;  //使用S口
    DDRS    = 0xE0;    //SCK0=1,MOSI=1,0x1110 0000
    MODRR   = 0x00;
    SPI0CR2 = 0x10;
    SPI0CR1 = 0x5e;
    SPI0BR  = 0x45; //BR=busclk/((SPPR + 1)· 2^(SPR + 1))=16000/(5*32)=100k
    SD_deSel();
}

//sd卡写命令
byte SD_Send_Cmd(byte cmd, dword arg)
{
    byte r1;
    byte retry = 0;

    (void)SPI_RdWr_Byte(0xff);
    SD_Sel();

    (void)SPI_RdWr_Byte(cmd | 0x40);          //分别写入命令
    (void)SPI_RdWr_Byte((byte)(arg >> 24));
    (void)SPI_RdWr_Byte((byte)(arg >> 16));
    (void)SPI_RdWr_Byte((byte)(arg >> 8));
    (void)SPI_RdWr_Byte((byte)(arg));
    (void)SPI_RdWr_Byte(0x95);
    r1 = SPI_RdWr_Byte(0xff);
    while(r1 == 0xff) //等待响应，
    {
        r1 = SPI_RdWr_Byte(0xff);
        if(++retry > 100) break;                //超时退出
    }
    SD_deSel();

    return r1;                                //返回状态值
}

//sd卡复位
byte SD_Reset(void)
{
    byte i;
    byte retry;
    byte r1 = 0;
    retry = 0;
    do
    {
        for(i = 0; i < 10; i++)
            (void)SPI_RdWr_Byte(0xff);
        r1 = SD_Send_Cmd(0, 0);                 //发idle命令
        if(++retry > 100) return 1;             //超时退出
    }
    while(r1 != 0x01);

    retry = 0;
    do
    {
        r1 = SD_Send_Cmd(1, 0);           //发active命令
        if(++retry > 100) return 1;       //超时退出
    }
    while(r1);

    r1 = SD_Send_Cmd(59, 0);            //关crc

    r1 = SD_Send_Cmd(16, 512);          //设扇区大小512

    return 0;                           //正常返回
}


#pragma CODE_SEG DEFAULT
//写一个扇区
byte Wt_Block(dword sector, byte *buffer)
{
    byte r1;
    word i;
    //byte retry=0;
    if(sector < 1) return 0xff;       //为了保护SD卡引导区，跳过该区
    r1 = SD_Send_Cmd(24, sector << 9); //写命令
    if(r1 != 0x00) return r1;

    SD_Sel();

    (void)SPI_RdWr_Byte(0xff);
    (void)SPI_RdWr_Byte(0xff);
    (void)SPI_RdWr_Byte(0xff);
    (void)SPI_RdWr_Byte(0xfe);        //发开始符

    for(i = 0; i < 512; i++)          //送512字节数据
    {
        (void)SPI_RdWr_Byte(*buffer++);
    }

    (void)SPI_RdWr_Byte(0xff);
    (void)SPI_RdWr_Byte(0xff);

    r1 = SPI_RdWr_Byte(0xff);

    if((r1 & 0x1f) != 0x05)           //等待是否成功
    {
        SD_deSel();
        return r1;
    }
    //等待操作完
    while(!SPI_RdWr_Byte(0xff));      //if(++retry > 50){SPI_CS_Deassert();return 1;}

    SD_deSel();

    return 0;
}

//读一个扇区
byte Rd_Block(dword sector, byte *buffer)
{
    byte r1;
    word i;
    //byte retry=0;

    r1 = SD_Send_Cmd(17, sector << 9); //读命令
    if(r1 != 0x00) 		return r1;

    SD_Sel();
    //等数据的开始
    while(SPI_RdWr_Byte(0xff) != 0xfe);//if(++retry > 50){SD_deSel();return 1;}

    for(i = 0; i < 512; i++) //读512个数据
    {
        *buffer++ = SPI_RdWr_Byte(0xff);
    }

    (void)SPI_RdWr_Byte(0xff);//伪crc
    (void)SPI_RdWr_Byte(0xff);
    SD_deSel();

    return 0;
}


#pragma CODE_SEG DEFAULT


