#pragma once

#include "DataStructures.h"

#ifdef __cplusplus
namespace SecretStash
{
#endif

//Read data into variables which are then returned as values

FolderVector GetFolderVectorFromBytes(ByteBuffer* byteBuffer);
UnicodeStringVector GetUnicodeStringVectorFromBytes(ByteBuffer* byteBuffer);

Folder GetFolderFromBytes(ByteBuffer* byteBuffer);
size_t GetUnsignedIntFromBytes(ByteBuffer* byteBuffer);

//UNICODE_STRING Related

UNICODE_STRING GetUnicodeStringFromBytes(ByteBuffer* byteBuffer);
USHORT GetUnsignedShortFromBytes(ByteBuffer* byteBuffer);
PWSTR GetWideCharArrayFromBytes(USHORT length, USHORT maxBufferSize, ByteBuffer* byteBuffer);

//Read data into pointers/buffers

void ReadFoldersFromBytes(FolderVector* folderVect, ByteBuffer* byteBuffer);
void ReadUnicodeStringsFromBytes(UnicodeStringVector* unicodeStringVector, ByteBuffer* byteBuffer);
void ReadBytesFromBuffer(BYTE* outputBuffer, size_t numBytesToRead, ByteBuffer* byteBuffer);

#ifdef __cplusplus
} /* namespace SecretStash */
#endif