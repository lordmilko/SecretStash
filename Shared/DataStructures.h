#pragma once

#if KERNEL_MODE
#include <fltKernel.h>
#include <dontuse.h>
#include <suppress.h>
#include <minwindef.h>
#else
#include "../SecretStashUI/UnicodeString.h"
#endif

#ifdef __cplusplus
namespace SecretStash
{
#endif

/*************************************************************************
    ByteBuffer
 *************************************************************************/

typedef struct _ByteBuffer {
    ULONG Position;
    ULONG Size;
    BYTE* Buffer;
} ByteBuffer;

#define NewByteBuffer() { 0, 0, NULL }

/*************************************************************************
    UNICODE_STRING Vector
 *************************************************************************/

typedef struct _UnicodeStringVector {
    UNICODE_STRING* data;
    size_t capacity;
    size_t size;
    size_t elementSize;
} UnicodeStringVector;

#define UnicodeStringVector() { NULL, 0, 0, 0 }

/*************************************************************************
    Folder
 *************************************************************************/

typedef struct _Folder {
    UNICODE_STRING VolumeDirPath;
    UNICODE_STRING PartitionDirPath;
    UnicodeStringVector FileNames;
} Folder;

/*************************************************************************
    Folder Vector
 *************************************************************************/

typedef struct _FolderVector {
    Folder* data;
    size_t capacity;
    size_t size;
    size_t elementSize;
} FolderVector;

#ifdef KERNEL_MODE

/*************************************************************************
    Communication Port Config
 *************************************************************************/

typedef struct _CommunicationPortConfig {
    PFLT_PORT ServerPort;
    PFLT_PORT ClientPort;
} CommunicationPortConfig;

/*************************************************************************
    SecretStash Global Config
 *************************************************************************/

typedef struct _SecretStashConfig {
    //Handle to the minifilter driver
    PFLT_FILTER Filter;
    CommunicationPortConfig ComPortConfig;
    FolderVector FolderVector;
    BOOLEAN RegDataLoaded;
} SecretStashConfig;

#endif /* KERNEL_MODE */

void DestroyFolderVector(FolderVector* folderVector);
void DestroyUnicodeString(UNICODE_STRING* unicodeString);

#ifdef __cplusplus
} /* namespace SecretStash */
#endif