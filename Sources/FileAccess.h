#ifndef __FILEACCESS_H__
#define __FILEACCESS_H__

#include "headers.h"

#define READONLY 0
#define WRITE 1

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */


    UINT8 Open(struct FileInfoStruct *pfi , INT8 *filepath , int Method);
    UINT8 ReadOneByteFromFile(struct FileInfoStruct *pfi, UINT32 offset, INT8 *pbuf);
    UINT8 ReadBytesFromFile(struct FileInfoStruct *pfi, UINT32 offset, UINT32 len, INT8 *pbuf);
    UINT32 WriteOneByteToFile(struct FileInfoStruct *pfi, INT8 *pbuf);
    UINT32 WriteBytesToFile(struct FileInfoStruct *pfi, UINT32 len, INT8 *pbuf);
    UINT8 DeleteFile(INT8 *filepath);
    UINT8 CloseFile(struct FileInfoStruct *pfi);

#ifdef __cplusplus
#if __cplusplus
}
#endif

#endif /* __cplusplus */

#endif __FILEACCESS_H__