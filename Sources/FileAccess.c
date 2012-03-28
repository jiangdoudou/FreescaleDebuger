#include "FileAccess.h"
#include "headers.h"
/**************************************************************************
 - 功能描述：打开一个文件
 - 函数属性：外部，使用户使用
 - 参数说明：pfi: FileInfoStruct类型的结构体指针，用来装载文件的参数信息
              比如文件的大小、文件的名称、文件的开始簇等等，以备后面使用
             filepath: 文件的路径，支持任意层目录 比如
              "\\dir1\\dir2\\dir3\\....\\test.txt"
			 Method：0 只读，1覆盖写或者创建
 - 返回说明：0：成功 1：文件不存在 2：目录不存在 
 - 注：打开文件不成功有很多原因，比如文件不存在、文件的某一级目录不存在
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
 - 功能描述：从文件的某一个位置处，读取一个字节
 - 函数属性：外部，使用户使用
 - 参数说明：pfi:FileInfoStruct类型的结构体指针，用于装载文件参数信息，文件
             读取的过程中，此结构体中的相关参数会更新，比如文件的当前偏移量、
             文件的当前扇区，文件的当前簇等等
             offset:读取的文职
             pbuf:数据缓冲区
 - 返回说明：读取到的实际的数据长度，如果读取失败，比如指定的偏移量大于了文件
             大小，则返回0
 - 注：在读取一个文件的数据前，必须先将该文件打开
 **************************************************************************/
UINT8 ReadOneByteFromFile(struct FileInfoStruct *pfi, UINT32 offset, INT8 *pbuf)
{
    return FAT32_Read_File(pfi, offset, 1, (UINT8 *)pbuf);
}

/**************************************************************************
 - 功能描述：从文件的某一个位置处，读取一定长度的数据，放入数据缓冲区中
 - 函数属性：外部，使用户使用
 - 参数说明：pfi:FileInfoStruct类型的结构体指针，用于装载文件参数信息，文件
             读取的过程中，此结构体中的相关参数会更新，比如文件的当前偏移量、
             文件的当前扇区，文件的当前簇等等
             offset:要定位的偏移量，要小于文件的大小
             len:要读取的数据的长度，如果len+offset大于文件的大小，则实际读
             取的数据量是从offset开始到文件结束
             pbuf:数据缓冲区
 - 返回说明：读取到的实际的数据长度，如果读取失败，比如指定的偏移量大于了文件
             大小，则返回0
 - 注：在读取一个文件的数据前，必须先将该文件打开
 **************************************************************************/
UINT8 ReadBytesFromFile(struct FileInfoStruct *pfi, UINT32 offset, UINT32 len, INT8 *pbuf)
{
    return FAT32_Read_File(pfi, offset, len, (UINT8 *)pbuf);
}

/**************************************************************************
 - 功能描述：向某一个文件追加数据
 - 函数属性：外部，使用户使用
 - 参数说明：pfi:指向FileInfoStruct类型的结构体，用于装载刚创建的文件的信息
             pbuf:指向数据缓冲区的指针
 - 返回说明：成功返回实际写入的数据长度，失败返回0
 - 注：追加数据失败很有可能是存储设备已经没有空间了，也就是找不到空闲簇了
 **************************************************************************/
UINT32 WriteOneByteToFile(struct FileInfoStruct *pfi, INT8 *pbuf)
{
    return FAT32_Add_Dat(pfi, 1, (UINT8 *)pbuf);
}

/**************************************************************************
 - 功能描述：向某一个文件追加数据
 - 函数属性：外部，使用户使用
 - 参数说明：pfi:指向FileInfoStruct类型的结构体，用于装载刚创建的文件的信息
             len:要追加的数据长度
             pbuf:指向数据缓冲区的指针
 - 返回说明：成功返回实际写入的数据长度，失败返回0
 - 注：追加数据失败很有可能是存储设备已经没有空间了，也就是找不到空闲簇了
 **************************************************************************/
UINT32 WriteBytesToFile(struct FileInfoStruct *pfi, UINT32 len, INT8 *pbuf)
{
    return FAT32_Add_Dat(pfi, len , (UINT8 *)pbuf);
}

/**************************************************************************
 - 功能描述：删除文件(支持任意层目录)
 - 函数属性：外部，使用户使用
 - 参数说明：filepath:文件路径 比如 "\\dir1\\dir2\\dir3\\....\\dirn\\test.txt"
 - 返回说明：1:文件或目录路径不存在 0:成功
 - 注：删除后的文件的FAT表中的簇链关系完全被破坏
 **************************************************************************/
UINT8 DeleteFile(INT8 *filepath)
{
    return FAT32_Del_File(filepath);
}

/**************************************************************************
 - 功能描述：文件关闭
 - 函数属性：外部，使用户使用
 - 参数说明：pfi:指向当前打开的文件的文件信息结构
 - 返回说明：0:成功
 - 注：无
 **************************************************************************/
UINT8 CloseFile(struct FileInfoStruct *pfi)
{
    return FAT32_File_Close(pfi);
}


