#include "Encode.h"
#include "../Shared/Vector.h"
#include "MemoryManagement.h"
#include "Debug.h"

/*************************************************************************
    Folder Encoders
 *************************************************************************/

void CopyFolderVectorToByteBuffer(FolderVector* folderVector, ByteBuffer* byteBuffer)
{
    byteBuffer->Size = CalculateRequiredByteBufferSize(folderVector);

    byteBuffer->Buffer = (BYTE*)ExAllocatePoolWithTag(PagedPool, byteBuffer->Size, BYTE_BUFFER_TAG);

    byteBuffer->Position = 0;

    CopyUnsignedIntToByteBuffer(&folderVector->capacity, byteBuffer);
    CopyUnsignedIntToByteBuffer(&folderVector->size, byteBuffer);

    CopyFoldersToByteBuffer(folderVector, byteBuffer);
}

void CopyFoldersToByteBuffer(FolderVector* folderVector, ByteBuffer* byteBuffer)
{
    for (size_t i = 0; i < folderVector->size; i++)
    {
        Folder* folder = FolderVectorGet(folderVector, i);

        CopyFolderToByteBuffer(folder, byteBuffer);
    }
}

void CopyFolderToByteBuffer(Folder* folder, ByteBuffer* byteBuffer)
{
    CopyUnicodeStringToByteBuffer(&folder->VolumeDirPath, byteBuffer);
    CopyUnicodeStringToByteBuffer(&folder->PartitionDirPath, byteBuffer);

    CopyUnicodeStringVectorToByteBuffer(&folder->FileNames, byteBuffer);
}

/*************************************************************************
    Unicode String Encoders
*************************************************************************/

void CopyUnicodeStringVectorToByteBuffer(UnicodeStringVector* unicodeStringVector, ByteBuffer* byteBuffer)
{
    CopyUnsignedIntToByteBuffer(&unicodeStringVector->capacity, byteBuffer);
    CopyUnsignedIntToByteBuffer(&unicodeStringVector->size, byteBuffer);

    for (size_t i = 0; i < unicodeStringVector->size; i++)
    {
        UNICODE_STRING* unicodeString = UnicodeStringVectorGet(unicodeStringVector, i);

        CopyUnicodeStringToByteBuffer(unicodeString, byteBuffer);
    }
}

void CopyUnicodeStringToByteBuffer(UNICODE_STRING* unicodeString, ByteBuffer* byteBuffer)
{
    CopyBytesToByteBuffer((BYTE*)&unicodeString->Length, sizeof(unicodeString->Length), byteBuffer);
    CopyBytesToByteBuffer((BYTE*)&unicodeString->MaximumLength, sizeof(unicodeString->MaximumLength), byteBuffer);
    CopyBytesToByteBuffer((BYTE*)unicodeString->Buffer, unicodeString->Length, byteBuffer);
}

/*************************************************************************
    Primative Data Type Encoders
*************************************************************************/

void CopyUnsignedIntToByteBuffer(size_t* unsignedInt, ByteBuffer* byteBuffer)
{
    CopyBytesToByteBuffer((BYTE*)unsignedInt, sizeof(unsignedInt), byteBuffer);
}

void CopyBytesToByteBuffer(BYTE* bytes, size_t NumberOfBytes, ByteBuffer* byteBuffer)
{
    for (size_t i = 0; i < NumberOfBytes; i++, byteBuffer->Position++)
    {
        byteBuffer->Buffer[byteBuffer->Position] = bytes[i];
    }
}

/*************************************************************************
    Data Structure Size Calculators
*************************************************************************/

ULONG CalculateRequiredByteBufferSize(FolderVector* folderVector)
{
    ULONG totalSize = 0;

    totalSize += sizeof(folderVector->capacity);
    totalSize += sizeof(folderVector->size);

    totalSize += CalculateSizeOfFolderStructs(folderVector);

    return totalSize;
}

ULONG CalculateSizeOfFolderStructs(FolderVector* folderVector)
{
    ULONG size = 0;

    for (size_t i = 0; i < folderVector->size; i++)
    {
        Folder* folder = FolderVectorGet(folderVector, i);

        size += SizeOfUnicodeStringInclBuffer(folder->VolumeDirPath);
        size += SizeOfUnicodeStringInclBuffer(folder->PartitionDirPath);

        size += CalculateSizeOfFolderFileNames(folder);
    }

    return size;
}
    
ULONG CalculateSizeOfFolderFileNames(Folder* folder)
{
    ULONG size = 0;

    size += sizeof(folder->FileNames.capacity);
    size += sizeof(folder->FileNames.size);

    for (size_t i = 0; i < folder->FileNames.size; i++)
    {
        UNICODE_STRING* fileName = UnicodeStringVectorGet(&folder->FileNames, i);

        size += SizeOfUnicodeStringInclBuffer(*fileName);
    }

    return size;
}