#pragma once

#include <fltKernel.h>
#include <dontuse.h>
#include <suppress.h>

#include "HideFiles.h"

/* Procedure for hiding specific files based on NTcloak by cgutman - https://github.com/cgutman/NTcloak/blob/master/CloakDrv/CloakDrv.c
 *
 * While NTcloak uses six separate switch cases in a single 600 line function, with each case almost identical but for
 * the data type of the FileInformationClass that is being manipulated, SecretStash takes a significantly more modular approach.
 * Significant factorizations have been performed to the original NTcloak function - both splitting each case up into multiple functions,
 * and using template system to reduce code duplication between different switch cases.
 *
 * As C doesn't support templates (and using C++ instead would result in templates being inserted in random segments) we use
 * a series of macros to automatically create the six sets of functions required for processing each FileInformationClass type
 */

//**************************************************************************************************************************************************\

#define CREATE_GETMOVELENGTH(directoryInfoType, fileInformationClass)                                                                               \
                                                                                                                                                    \
    ULONG fileInformationClass##_GetMoveLength(directoryInfoType* fileDirInfo)                                                                      \
    {                                                                                                                                               \
        ULONG moveLength = 0;                                                                                                                       \
                                                                                                                                                    \
        directoryInfoType* nextFileDirInfo = (directoryInfoType*)((PUCHAR)fileDirInfo + fileDirInfo->NextEntryOffset);                              \
                                                                                                                                                    \
        while (nextFileDirInfo->NextEntryOffset != 0)                                                                                               \
        {                                                                                                                                           \
            /* We use the FIELD_OFFSET macro because FileName is declared as FileName[1] which means that
             * we can't just do sizeof(FILE_DIRECTORY_INFORMATION) + nextFileDirInfo->FileNameLength. */                                            \
            moveLength += FIELD_OFFSET(directoryInfoType, FileName) + nextFileDirInfo->FileNameLength;                                              \
            nextFileDirInfo = (directoryInfoType*)((PUCHAR)nextFileDirInfo + nextFileDirInfo->NextEntryOffset);                                     \
        }                                                                                                                                           \
                                                                                                                                                    \
        /* Add the final entry */                                                                                                                   \
        moveLength += FIELD_OFFSET(directoryInfoType, FileName) + nextFileDirInfo->FileNameLength;                                                  \
                                                                                                                                                    \
        return moveLength;                                                                                                                          \
    }

//**************************************************************************************************************************************************\

#define CREATE_PROCESSFIRSTENTRY(directoryInfoType, fileInformationClass)                                                                           \
                                                                                                                                                    \
    CREATE_GETMOVELENGTH(directoryInfoType, fileInformationClass)                                                                                                         \
                                                                                                                                                    \
    void fileInformationClass##_ProcessFirstEntry(PFLT_CALLBACK_DATA Data, directoryInfoType* fileDirInfo)                                          \
    {                                                                                                                                               \
        if (fileDirInfo->NextEntryOffset != 0)                                                                                                      \
        {                                                                                                                                           \
            /* This is the first entry
             * Calculate the length of the whole list */                                                                                            \
             ULONG moveLength = fileInformationClass##_GetMoveLength(fileDirInfo);                                                                     \
                                                                                                                                                    \
            /* We need to move everything forward.
             * NOTE: RtlMoveMemory (memove) is required for overlapping ranges like this one. */                                                    \
            RtlMoveMemory(fileDirInfo, (PUCHAR)fileDirInfo + fileDirInfo->NextEntryOffset, moveLength);                                             \
        }                                                                                                                                           \
        else                                                                                                                                        \
        {                                                                                                                                           \
            /* This is the first and last entry, so there's nothing to return */                                                                    \
            Data->IoStatus.Status = STATUS_NO_MORE_ENTRIES;                                                                                         \
            return;                                                                                                                                 \
        }                                                                                                                                           \
    }

//**************************************************************************************************************************************************\

#define CREATE_PROCESSSUBSEQUENTENTRIES(directoryInfoType, fileInformationClass)                                                                    \
                                                                                                                                                    \
    void fileInformationClass##_ProcessSubsequentEntries(directoryInfoType* lastFileDirInfo, directoryInfoType* fileDirInfo)                        \
    {                                                                                                                                               \
        if (fileDirInfo->NextEntryOffset != 0)                                                                                                      \
        {                                                                                                                                           \
            /* This is not the first entry
             * Just point the last info's offset to the next info */                                                                                \
            lastFileDirInfo->NextEntryOffset += fileDirInfo->NextEntryOffset;                                                                       \
        }                                                                                                                                           \
        else                                                                                                                                        \
        {                                                                                                                                           \
            /* This is the last entry */                                                                                                            \
            lastFileDirInfo->NextEntryOffset = 0;                                                                                                   \
        }                                                                                                                                           \
    }                                                                                                                                               \

//**************************************************************************************************************************************************\

#define CREATE_PROCESSFILENAMEENTRIES(directoryInfoType, fileInformationClass)                                                                      \
                                                                                                                                                    \
    CREATE_PROCESSSUBSEQUENTENTRIES(directoryInfoType, fileInformationClass)                                                                        \
    CREATE_PROCESSFIRSTENTRY(directoryInfoType, fileInformationClass)                                                                               \
                                                                                                                                                    \
    void fileInformationClass##_ProcessFileNameEntries(PFLT_CALLBACK_DATA Data, directoryInfoType* lastFileDirInfo, directoryInfoType* fileDirInfo) \
    {                                                                                                                                               \
        if (lastFileDirInfo != NULL)                                                                                                                \
        {                                                                                                                                           \
            fileInformationClass##_ProcessSubsequentEntries(lastFileDirInfo, fileDirInfo);                                                          \
        }                                                                                                                                           \
        else                                                                                                                                        \
        {                                                                                                                                           \
            fileInformationClass##_ProcessFirstEntry(Data, fileDirInfo);                                                                            \
        }                                                                                                                                           \
    }

//**************************************************************************************************************************************************\

#define CREATE_PROCESSFILEDIRECTORYINFORMATION(directoryInfoType, fileInformationClass)                                                             \
                                                                                                                                                    \
    CREATE_PROCESSFILENAMEENTRIES(directoryInfoType, fileInformationClass)                                                                          \
                                                                                                                                                    \
    void fileInformationClass##_HideFiles(PFLT_CALLBACK_DATA Data)                                                                                  \
    {                                                                                                                                               \
        UNICODE_STRING fileName;                                                                                                                    \
                                                                                                                                                    \
        directoryInfoType* lastFileDirInfo = NULL;                                                                                                  \
        directoryInfoType* fileDirInfo = (directoryInfoType*)Data->Iopb->Parameters.DirectoryControl.QueryDirectory.DirectoryBuffer;                \
                                                                                                                                                    \
        while(lastFileDirInfo != fileDirInfo)                                                                                                       \
        {                                                                                                                                           \
            /* Create a unicode string from file name so we can use FsRtl */                                                                        \
            fileName.Buffer = fileDirInfo->FileName;                                                                                                \
            fileName.Length = (USHORT)fileDirInfo->FileNameLength;                                                                                  \
            fileName.MaximumLength = fileName.Length;                                                                                               \
                                                                                                                                                    \
            /* Check if this is a match on our hide file name */                                                                                    \
            /*if (FsRtlIsNameInExpression(gHideFileName, &fileName, TRUE, NULL)) */                                                                     \
            if(ShouldHideFile(fileName, Data))                                                                                                                  \
            {                                                                                                                                       \
                fileInformationClass##_ProcessFileNameEntries(Data, lastFileDirInfo, fileDirInfo);                                                  \
                                                                                                                                                    \
                if (Data->IoStatus.Status == STATUS_NO_MORE_ENTRIES)                                                                                \
                    return;                                                                                                                         \
            }                                                                                                                                       \
                                                                                                                                                    \
            /* Advance to the next directory info */                                                                                                \
            lastFileDirInfo = fileDirInfo;                                                                                                          \
                                                                                                                                                    \
            fileDirInfo = (directoryInfoType*)((PUCHAR)fileDirInfo + fileDirInfo->NextEntryOffset);                                                 \
                                                                                                                                                    \
        }                                                                                                                                           \
    }

//**************************************************************************************************************************************************\

    CREATE_PROCESSFILEDIRECTORYINFORMATION(FILE_DIRECTORY_INFORMATION, FileDirectoryInformation)
    CREATE_PROCESSFILEDIRECTORYINFORMATION(FILE_FULL_DIR_INFORMATION, FileFullDirectoryInformation)
    CREATE_PROCESSFILEDIRECTORYINFORMATION(FILE_NAMES_INFORMATION, FileNamesInformation)
    CREATE_PROCESSFILEDIRECTORYINFORMATION(FILE_BOTH_DIR_INFORMATION, FileBothDirectoryInformation)
    CREATE_PROCESSFILEDIRECTORYINFORMATION(FILE_ID_BOTH_DIR_INFORMATION, FileIdBothDirectoryInformation)
    CREATE_PROCESSFILEDIRECTORYINFORMATION(FILE_ID_FULL_DIR_INFORMATION, FileIdFullDirectoryInformation)

    //FIND OUT WHY WE CANT DBG_PRINT FROM ENCODE.C