#include "Decode.h"
#include "Vector.h"

#if KERNEL_MODE
    #include "../SecretStash/MemoryManagement.h"
#endif

FolderVector GetFolderVectorFromBytes(ByteBuffer* byteBuffer)
{
    byteBuffer->Position = 0;

    FolderVector folderVect;

    //The first few bytes of the byteBuffer indicate the vector's previous capacity
    size_t vectCapacity = GetUnsignedIntFromBytes(byteBuffer);

    //Initialize our vector
    FolderVectorInitialize(&folderVect, vectCapacity);

    ReadFoldersFromBytes(&folderVect, byteBuffer);

    return folderVect;
}

void ReadFoldersFromBytes(FolderVector* folderVect, ByteBuffer* byteBuffer)
{
    //The next sequence of bytes indicate the total number of items that were contained in the vector.
    size_t vectSize = GetUnsignedIntFromBytes(byteBuffer);

    //Read each item from the vector
    for (size_t i = 0; i < vectSize; i++)
    {
        Folder folder = GetFolderFromBytes(byteBuffer);

        FolderVectorInsert(folderVect, folder);
    }
}

Folder GetFolderFromBytes(ByteBuffer* byteBuffer)
{
    Folder folder;

    folder.VolumeDirPath = GetUnicodeStringFromBytes(byteBuffer);
    folder.PartitionDirPath = GetUnicodeStringFromBytes(byteBuffer);
    folder.FileNames = GetUnicodeStringVectorFromBytes(byteBuffer);

    return folder;
}

UnicodeStringVector GetUnicodeStringVectorFromBytes(ByteBuffer* byteBuffer)
{
    UnicodeStringVector unicodeStringVector;

    //Get the previous capacity of the vector
    size_t vectCapacity = GetUnsignedIntFromBytes(byteBuffer);

    //Initialize our vector
    UnicodeStringVectorInitialize(&unicodeStringVector, vectCapacity);

    ReadUnicodeStringsFromBytes(&unicodeStringVector, byteBuffer);

    return unicodeStringVector;
}

void ReadUnicodeStringsFromBytes(UnicodeStringVector* unicodeStringVector, ByteBuffer* byteBuffer)
{
    //Get the number of items that were contained in the vector
    size_t vectSize = GetUnsignedIntFromBytes(byteBuffer);

    for (size_t i = 0; i < vectSize; i++)
    {
        UNICODE_STRING unicodeString = GetUnicodeStringFromBytes(byteBuffer);

        UnicodeStringVectorInsert(unicodeStringVector, unicodeString);
    }
}

//Lowest level ByteBuffer Helpers

size_t GetUnsignedIntFromBytes(ByteBuffer* byteBuffer)
{
    size_t output = 0;

    BYTE outputBuffer[sizeof(output)];
    ReadBytesFromBuffer(outputBuffer, sizeof(output), byteBuffer);

    output = (size_t)*outputBuffer;

    return output;
}

void ReadBytesFromBuffer(BYTE* outputBuffer, size_t numBytesToRead, ByteBuffer* byteBuffer)
{
    //we need to be checking we havent gone past bytebuffer->size

    for (size_t i = 0; i < numBytesToRead; i++, byteBuffer->Position++)
    {
        if (byteBuffer->Position < byteBuffer->Size)
            outputBuffer[i] = byteBuffer->Buffer[byteBuffer->Position];
        else
        {
            //some kind of assert
        }
    }
}

//UNICODE_STRING Related

UNICODE_STRING GetUnicodeStringFromBytes(ByteBuffer* byteBuffer)
{
    UNICODE_STRING unicodeString;

    unicodeString.Length = GetUnsignedShortFromBytes(byteBuffer);
    unicodeString.MaximumLength = GetUnsignedShortFromBytes(byteBuffer);
    unicodeString.Buffer = GetWideCharArrayFromBytes(unicodeString.Length, unicodeString.MaximumLength, byteBuffer);

    return unicodeString;
}

USHORT GetUnsignedShortFromBytes(ByteBuffer* byteBuffer)
{
    USHORT output = 0;

    BYTE outputBuffer[sizeof(output)];
    ReadBytesFromBuffer(outputBuffer, sizeof(output), byteBuffer);

    output = (USHORT)*outputBuffer;

    return output;
}

PWSTR GetWideCharArrayFromBytes(USHORT length, USHORT maxBufferSize, ByteBuffer* byteBuffer)
{
    //Allocate space on the heap for our string to be stored. This space will be deallocated when the driver is unloaded

#if KERNEL_MODE
    BYTE* stringBuffer = (BYTE*)ExAllocatePoolWithTag(PagedPool, maxBufferSize * sizeof(WCHAR), UNICODE_STRING_TAG);
#else
    BYTE* stringBuffer = (BYTE*)malloc(maxBufferSize * sizeof(WCHAR));
#endif

    //Read the string into its buffer
    ReadBytesFromBuffer(stringBuffer, length, byteBuffer);

    return (PWSTR)stringBuffer;
}