//--------------------------------------------------------------------------*
// 文件名: MCUInit.c                                                        *
// 说  明: 芯片初始化设置:通过设置CLKSEL寄存器，确定内部总线的时钟源;通过   *
//         PLL编程,设置PLLCLK;通过设置INTCR和COPCTL寄存器,分别决定是否允许  *
//         IRQ中断和看门狗。                                                *
//         (1)外部晶振=16Mhz,根据给定的总线时钟目标值设置参数SYNR、REFDV    *
//            总线时钟频率推荐值16、20、32、40、48、60MHz                   *
//         (3)禁止IRQ中断                                                   *
//         (4)禁止看门狗                                                    *
//--------------------------------------------------------------------------*
            
//头文件包含，及宏定义区
 
   // 头文件包含
   #include "MCUInit.h"    //芯片初始化头文件
    
   //-----------------------------------------------------------------------*
   //函数名: Light_Init时钟设置函数定义                                     *
   //功  能: 基于16MHz的外部晶振，设置总线时钟频率                          *
   //参  数: fbus为目标总线频率值，推荐值为16、20、32、40、48、60MHz        *
   //返  回: 无                                                             * 
   //-----------------------------------------------------------------------*
   void SetBusCLK_M(uint8 fbus)
   {   
        CLKSEL=0x00;          // 不加载IPLL到系统
        REFDV=0x00 | 0x07;    // REFFRQ[1:0];REFDIV[5:0]
        switch(fbus)          // BUS CLOCK
        {
          case 16: SYNR =0x00 | 0x07;break;
          case 20: SYNR =0x00 | 0x09;break;
          case 32: SYNR =0x40 | 0x0F;break;
          case 40: SYNR =0x40 | 0x13;break;
          case 48: SYNR =0xC0 | 0x17;break;
          case 60: SYNR =0xC0 | 0x1D;break;
          default: break;
        }  
        PLLCTL_PLLON=1;           //启动IPLL
        _asm(nop);            
        _asm(nop);
        while(!(CRGFLG_LOCK==1)); //等待IPLL稳定
        POSTDIV=0x00;             //POSTDIV[4:0], fPLL= fVCO/(2xPOSTDIV)
                                  //若POSTDIV = 0x00，则fPLL = fVCO 
        CLKSEL_PLLSEL =1;         //加载IPLL到系统 
   }

   //芯片的初始化
   void MCUInit(uint8 fbus)
   {
        SetBusCLK_M(fbus);    //设置总线频率       
        IRQCR_IRQEN = 0;      //禁止IRQ中断(默认开)  
        COPCTL = 0x00;        //COPCTL[2:0] = 000禁止看门狗
   }                       
