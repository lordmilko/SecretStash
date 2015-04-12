#pragma once

#include "DataStructures.h"
#include "Vector.h"

#if KERNEL_MODE
#include "../SecretStash/MemoryManagement.h"
#else
#include <malloc.h>
#endif

#ifdef __cplusplus
namespace SecretStash
{
#endif

void DestroyFolderVector(FolderVector* folderVector)
{
    for (size_t i = 0; i < folderVector->size; i++)
    {
        Folder* folder = FolderVectorGet(folderVector, i);

        DestroyUnicodeString(&folder->VolumeDirPath);
        DestroyUnicodeString(&folder->PartitionDirPath);

        for (size_t j = 0; j < folder->FileNames.size; j++)
        {
            UNICODE_STRING* fileName = UnicodeStringVectorGet(&folder->FileNames, j);

            DestroyUnicodeString(fileName);
        }

        UnicodeStringVectorRemove(&folder->FileNames);
    }

    FolderVectorRemove(folderVector);
}

void DestroyUnicodeString(UNICODE_STRING* unicodeString)
{
#if KERNEL_MODE
    ExFreePoolWithTag(unicodeString->Buffer, UNICODE_STRING_TAG);
#else
    free(unicodeString->Buffer);
#endif

    unicodeString->Length = 0;
    unicodeString->MaximumLength = 0;
}

#ifdef __cplusplus
} /* namespace SecretStash */
#endif