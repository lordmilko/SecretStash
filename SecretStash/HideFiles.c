#include "HideFiles.h"
#include "../Shared/Vector.h"
#include "MemoryManagement.h"
#include "FileInformationClass_HideFileFunctionTemplates.h"
#include "Debug.h"

extern SecretStashConfig secretStashConfig;

//check whether a directory exists in the foldervector
BOOLEAN ShouldHideFiles(PFLT_CALLBACK_DATA Data)
{
    //in any files to hide we should check if any files have been configured for hiding in this folder
    BOOLEAN shouldHideFile = FolderContainsFilesToHide(Data) && (Data->Iopb->MinorFunction == IRP_MN_QUERY_DIRECTORY) && IsRelevantFileInfoQuery(Data);

    return shouldHideFile;
}

BOOLEAN FolderContainsFilesToHide(PFLT_CALLBACK_DATA Data)
{
    BOOLEAN filesToHide = FALSE;

    UNICODE_STRING directory;

    GetDirectoryName(Data, &directory);

    for (size_t i = 0; filesToHide == FALSE && i < secretStashConfig.FolderVector.size; i++)
    {
        Folder* folder = FolderVectorGet(&secretStashConfig.FolderVector, i);

        if (RtlEqualUnicodeString(&directory, &folder->VolumeDirPath, FALSE))
        {
            filesToHide = TRUE;
        }
    }

    DestroyUnicodeString(&directory);

    return filesToHide;
}

NTSTATUS GetDirectoryName(PFLT_CALLBACK_DATA Data, PUNICODE_STRING directoryName)
{
    PFLT_FILE_NAME_INFORMATION nameInfo;

    NTSTATUS status = FltGetFileNameInformation(Data, (FLT_FILE_NAME_OPENED | FLT_FILE_NAME_QUERY_DEFAULT), &nameInfo);

    if (NT_SUCCESS(status))
    {
        status = FltParseFileNameInformation(nameInfo);

        if (NT_SUCCESS(status))
        {
            directoryName->Length = 0;
            directoryName->Buffer = (WCHAR*)ExAllocatePoolWithTag(PagedPool, nameInfo->Name.Length * sizeof(WCHAR), UNICODE_STRING_TAG);
            directoryName->MaximumLength = nameInfo->Name.Length;

            status = RtlUnicodeStringCopy(directoryName, &nameInfo->Name);
        }
    }

    if (nameInfo != NULL)
    {
        FltReleaseFileNameInformation(nameInfo);
    }

    return status;
}

BOOLEAN IsRelevantFileInfoQuery(PFLT_CALLBACK_DATA Data)
{
    BOOLEAN relevantQuery = FALSE;

    switch (Data->Iopb->Parameters.DirectoryControl.QueryDirectory.FileInformationClass)
    {
    case FileIdFullDirectoryInformation:
    case FileIdBothDirectoryInformation:
    case FileBothDirectoryInformation:
    case FileDirectoryInformation:
    case FileFullDirectoryInformation:
    case FileNamesInformation:
        relevantQuery = TRUE;
    }

    return relevantQuery;
}

void ProcessFileInfoQuery(PFLT_CALLBACK_DATA Data)
{
    switch (Data->Iopb->Parameters.DirectoryControl.QueryDirectory.FileInformationClass)
    {
    case FileDirectoryInformation:
        FileDirectoryInformation_HideFiles(Data);
        break;
    case FileFullDirectoryInformation:
        FileFullDirectoryInformation_HideFiles(Data);
        break;
    case FileNamesInformation:
        FileNamesInformation_HideFiles(Data);
        break;
    case FileBothDirectoryInformation:
        FileBothDirectoryInformation_HideFiles(Data);
        break;
    case FileIdBothDirectoryInformation:
        FileIdBothDirectoryInformation_HideFiles(Data);
        break;
    case FileIdFullDirectoryInformation:
        FileIdFullDirectoryInformation_HideFiles(Data);
        break;
    default:
        NT_ASSERT(FALSE);
    }
}

/*************************************************************************
    Identify files that need to be hidden -----------------------------------------shouldhidefile is called by a fileinformationclass function - if we've gotten this far this means we KNOW theres a folder that has files to hide in it
*************************************************************************/

//Check whether this specific file needs to be hidden
BOOLEAN ShouldHideFile(UNICODE_STRING filename, PFLT_CALLBACK_DATA Data)
{
    BOOLEAN hideFile = FALSE;

    UNICODE_STRING directory;   
    NTSTATUS status = GetDirectoryName(Data, &directory);

    if (NT_SUCCESS(status))
    {
        DBG_PRINT("SecretStash!ShouldHideFile: Directory is %wZ, Filename is %wZ", &directory, &filename);

        hideFile = CheckFileToHideAgainstFolderVector(&directory, &filename);

        DestroyUnicodeString(&directory);
    }
    else
        NT_ASSERT(FALSE); ////////////////////////////////////handle this better somehow

    return hideFile;
}

BOOLEAN CheckFileToHideAgainstFolderVector(UNICODE_STRING* directory, UNICODE_STRING* filename)
{
    BOOLEAN hideFile = FALSE;

    for (size_t i = 0; i < secretStashConfig.FolderVector.size && hideFile == FALSE; i++)
    {
        Folder* folder = FolderVectorGet(&secretStashConfig.FolderVector, i);

        CheckFileToHideAgainstFolder(folder, directory, filename, &hideFile);
    }

    return hideFile;
}

void CheckFileToHideAgainstFolder(Folder* folder, UNICODE_STRING* directory, UNICODE_STRING* filename, BOOLEAN* hideFile)
{
    if (RtlEqualUnicodeString(directory, &folder->VolumeDirPath, FALSE))
    {
        CheckFileToHideAgainstFolderFileNames(folder, directory, filename, hideFile);
    }
}

void CheckFileToHideAgainstFolderFileNames(Folder* folder, UNICODE_STRING* directory, UNICODE_STRING* filename, BOOLEAN* hideFile)
{
    for (size_t j = 0; j < folder->FileNames.size; j++)
    {
        UNICODE_STRING* vectFileName = UnicodeStringVectorGet(&folder->FileNames, j);

        if (RtlEqualUnicodeString(filename, vectFileName, FALSE))
        {
            DBG_PRINT("SecretStash!ShouldHideFile: Hiding %wZ\\%wZ!", &directory, &filename);
            *hideFile = TRUE;
        }
    }
}