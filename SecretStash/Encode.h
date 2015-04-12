#pragma once

#include <fltKernel.h>
#include <ntstrsafe.h>
#include <dontuse.h>
#include <suppress.h>

#include "../Shared/DataStructures.h"

#define SizeOfUnicodeStringInclBuffer(str) (sizeof((str).Length) + sizeof((str).MaximumLength) + (str).Length)

/*************************************************************************
    Folder Encoders
 *************************************************************************/

void CopyFolderVectorToByteBuffer(FolderVector* folderVector, ByteBuffer* byteBuffer);
void CopyFoldersToByteBuffer(FolderVector* folderVector, ByteBuffer* byteBuffer);
void CopyFolderToByteBuffer(Folder* folder, ByteBuffer* byteBuffer);

/*************************************************************************
    Unicode String Encoders
*************************************************************************/

void CopyUnicodeStringVectorToByteBuffer(UnicodeStringVector* unicodeStringVector, ByteBuffer* byteBuffer);

void CopyUnicodeStringToByteBuffer(UNICODE_STRING* unicodeString, ByteBuffer* byteBuffer);

/*************************************************************************
    Primative Data Type Encoders
*************************************************************************/

void CopyUnsignedIntToByteBuffer(size_t* unsignedInt, ByteBuffer* byteBuffer);

void CopyBytesToByteBuffer(BYTE* bytes, size_t NumberOfBytes, ByteBuffer* byteBuffer);

/*************************************************************************
    Data Structure Size Calculators
*************************************************************************/

ULONG CalculateRequiredByteBufferSize(FolderVector* folderVector);
ULONG CalculateSizeOfFolderStructs(FolderVector* folderVector);
ULONG CalculateSizeOfFolderFileNames(Folder* folder);