//--------------------------------------------------------------------------*
//文件名: SCI.c                                                             *
//说  明: SCI构件函数源文件                                                 *
//--------------------------------------------------------------------------*

//头文件包含，及宏定义区

    //头文件包含
    #include         "SCI.h"                    //SCI串口构件头文件

//构件函数实现

    //----------------------------------------------------------------------*
    //函数名: SCIInit                                                       *
    //功  能: 初始化SCIx模块。x代表0，1                                     *
    //参  数: uint8 SCINo: 第SCINo个SCI模块。其中SCINo取值为0，1            *
    //                     如果SCINo大于1，则认为是1                        *
    //        uint8 sysclk: 系统总线时钟，以MHz为单位                       *
    //        uint16 baud:   波特率，如 4800，9600，19200，3840             * 
    //                      其他的波特率在MC9SO8AW60的评估板上没有测试过，  *
    //                      若要使用该函数，请自行测试，一般来说，速度慢一  *
    //                      点，通信会更稳定                                *
    //返  回: 无                                                            *
    //说  明: SCINo=0表示使用SCI0模块，依此类推。                           *
    //----------------------------------------------------------------------*
    void SCIInit(uint8 SCINo, uint8 sysclk, uint16 baud)
    {
        uint8 t;
        uint16 ubgs=0;
        if(SCINo>1) SCINo=1;                          //串口号错误输入处理
        
        
        //计算波特率并设置:ubgs = fsys/(波特率*16)(其中fsys=sysclk*1000000)
        ubgs = sysclk*(10000/(baud/100))/16;//理解参考上一行，此处是为了运算
                                                       //初始化相应寄存器
        switch (SCINo)
        {
          case 0:
                                                         //设置波特率
                
                SCI0BDH = (uint8)((ubgs & 0xFF00) >> 8); //给高8位赋值
                SCI0BDL = (uint8)(ubgs & 0x00FF);        //给低8位赋值
                
                SCI0CR1 = 0x00;      //设置允许SCI,正常码输出,8位数据,无校验
                t = SCI0DRL;         //读数据寄存器(清0)
                t = SCI0SR1;         //读状态寄存器(清0)
                SCI0CR2 = 0x0C;      //允许SCI0接收和发送 查询方式
                break;
          case 1:
                                                          //设置波特率
                
                SCI1BDH = (uint8)((ubgs & 0xFF00) >> 8);  //给高8位赋值
                SCI1BDL = (uint8)(ubgs & 0x00FF);         //给低8位赋值
                
                SCI1CR1 = 0x00;     //设置允许SCI,正常码输出,8位数据,无校验
                t = SCI1DRL;                  //读数据寄存器(清0)
                t = SCI1SR1;                  //读状态寄存器(清0)
                SCI1CR2 = 0x0C;               //允许SCI0接收和发送 查询方式
                break;
        }
    }

    //----------------------------------------------------------------------*
    //函数名: SCISend1                                                      *
    //功  能: 串行发送1个字节                                               *
    //参  数: uint8 SCINo: 第SCINo个SCI模块，其中SCINo取值为0，1            *
    //        uint8 ch:    要发送的字节                                     *
    //返  回: 无                                                            *
    //说  明: SCINo=0表示使用SCI0模块，依此类推                             *
    //----------------------------------------------------------------------*
    void SCISend1(uint8 SCINo, uint8 ch)
    {
        uint16 k;
        if(SCINo > 1)
        {  
            SCINo = 1;                //若传进的通道号大于2，则按照2来接收
        }

                                                   //根据通道号发送数据
        switch (SCINo)
        {
          case 0:
                for (k = 0; k < 0xfbbb; k++)       //有时间限制
                {
                                                   //判断发送缓冲区是否为空
                  if ((ReSendStatusR0 & (1<<SendTestBit)) != 0)
                  { 
                      ReSendDataR0 = ch;
                      break;
                  }
                }
                break;
          case 1:
                for (k = 0; k < 0xfbbb; k++)      //有时间限制
                {
                                                  //判断发送缓冲区是否为空
                  if ((ReSendStatusR1 & (1<<SendTestBit)) != 0)
                  { 
                      ReSendDataR1 = ch;
                      break;
                  }
                }
                break;
        }
    }

    //----------------------------------------------------------------------*
    //函数名: SCISendN                                                      *
    //功  能: 串行发送N个字节                                               *
    //参  数: uint8 SCINo: 第SCINo个SCI模块，其中SCINo取值为0，1            *
    //        uint8 n:      发送的字节数                                    *
    //        uint8 ch[]:   待发送的数据                                    *
    //返  回: 无                                                            *
    //说  明: SCINo=0表示使用SCI0模块，依此类推                             *
    //        调用了SCISend1函数                                            *
    //----------------------------------------------------------------------*
    void SCISendN(uint8 SCINo, uint8 n, uint8 ch[])
    {
        uint8 i;
        for (i=0; i<n; i++)
            SCISend1(SCINo,ch[i]);
    }

    //----------------------------------------------------------------------*
    //函数名: SCIRe1                                                        *
    //功  能: 从串口接收1个字节的数据                                       *
    //参  数: uint8 SCINo: 第SCINo个SCI模块，其中SCINo取值为0，1            *
    //返  回: 接收到的数(若接收失败，返回0xff)                              *
    //        uint8 *p:     接收成功标志的指针(0表示成功，1表示不成功)      *
    //说  明: 参数*p带回接收标志，*p = 0，收到数据；*p = 1，未收到数据      *
    //说  明: SCINo=0表示使用SCI0模块，依此类推                             *
    //----------------------------------------------------------------------*
    uint8 SCIRe1(uint8 SCINo, uint8 *p)
    {
        uint16 k;
        uint8 i;
        
        if(SCINo > 1)
        {  
            SCINo = 1;                 //若传进的通道号大于2，则按照2来接收
        }
        
                                                       //根据通道号发送数据
        switch (SCINo)
        {
          case 0:
                for (k=0; k<0xfbbb; k++)
                                                      //判断接收缓冲区是否满
                  if ((ReSendStatusR0 & (1<<ReTestBit)) != 0)
            	    {
            	        i = ReSendDataR0;
            	        *p = 0x00; 
            	        break;
            	    }
            	                                         //接收失败    
                if (k >= 0xfbbb)
                {
                    i=0xff;
            	    *p=0x01;
                }
                return i;                              //返回接收到的数据
                break;
          case 1:
                for (k=0; k<0xfbbb; k++)
                                                      //判断接收缓冲区是否满
                  if ((ReSendStatusR1 & (1<<ReTestBit)) != 0)
            	    {
            	        i = ReSendDataR1;
            	        *p = 0x00; 
            	        break;
            	    }
            	                                         //接收失败    
                if (k >= 0xfbbb)
                {
                    i=0xff;
            	    *p=0x01;
                }
                return i;                              //返回接收到的数据
                break;
        }
    }

    //----------------------------------------------------------------------*
    //函数名: SCIReN                                                        *
    //功  能: 从串口接收N个字节的数据                                       *
    //参  数: uint8 SCINo: 第SCINo个SCI模块，其中SCINo取值为0，1            *
    //        uint8 n:      要接收的字节数                                  *
    //        uint8 ch[]:   存放接收数据的数组                              *
    //返  回: 接收标志= 0 接收成功，= 1 接收失败                            *
    //说  明: SCINo=0表示使用SCI1模块，依此类推                             *
    //        调用了SCIRe1函数                                              *
    //----------------------------------------------------------------------*
    uint8 SCIReN(uint8 SCINo, uint8 n, uint8 ch[])
    {
        uint8 m;
        uint8 fp;
        m = 0;
                                                           //接收n个数据
        while (m<n)
        {
    	    ch[m] = SCIRe1(SCINo,&fp);
    	    if (fp == 1) return 1;       //只要有1个字节数据没接收到就返回报错
    	    m++;
        }
        return 0;
    }

    //----------------------------------------------------------------------*
    //函数名: SCISendString                                                 *
    //功  能: 串口传输字符串                                                *
    //参  数: uint8 SCINo: 第SCINo个SCI模块，其中SCINo取值为1，2            *
    //        char *p: 要传输的字符串的指针                                 *
    //返  回: 无                                                            *
    //说  明: 字符串以'\0'结束                                              *
    //        调用了SCISend1 函数                                           *
    //----------------------------------------------------------------------*
    void SCISendString(uint8 SCINo,char *p)
    {
        uint32 k;    
        if(SCINo > 1)
        {  
            SCINo = 1;                  //若传进的通道号大于2，则按照2来接收
        }
        if(p == 0) return;
        for(k = 0; p[k] != '\0'; ++k)
        {
            SCISend1(SCINo,p[k]);
        }
    }