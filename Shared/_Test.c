#include "_Test.h"
#include "Vector.h"

#ifdef __cplusplus
namespace SecretStash
{
#endif

#ifdef KERNEL_MODE
#include "Registry.h"
#include "Encode.h"
#include "Debug.h"
#include "MemoryManagement.h"
#include "HideFiles.h"

void InsertTemporaryRegData(PUNICODE_STRING RegistryPath)
{
    DBG_PRINT("################# INSERTING TEMPORARY REGISTRY DATA #################");

    FolderVector folderVect;
    FolderVectorInitialize(&folderVect, DEFAULT_VECTOR_CAPACITY);

    AddNewFolder(&folderVect);
    PrintFolderValues(&folderVect);

    ByteBuffer byteBuffer = NewByteBuffer();

    CopyFolderVectorToByteBuffer(&folderVect, &byteBuffer);

    WriteRegistryValue(RegistryPath, L"Data", REG_BINARY, byteBuffer);

    ExFreePoolWithTag(byteBuffer.Buffer, BYTE_BUFFER_TAG);
}


void PrintFolderValues(FolderVector* folderVector)
{
    for (size_t i = 0; i < folderVector->size; i++)
    {
        Folder* folder = FolderVectorGet(folderVector, i);

        DBG_PRINT("%wZ\n", &folder->VolumeDirPath);
        DBG_PRINT("%wZ\n", &folder->PartitionDirPath);

        for (size_t j = 0; j < folder->FileNames.size; j++)
        {
            UNICODE_STRING* fileName = UnicodeStringVectorGet(&folder->FileNames, j);

            DBG_PRINT("%wZ\n", fileName);
        }
    }
}

void test(PFLT_CALLBACK_DATA Data)
{
    //get file name. look at delete driver to see how this is done

    //-------------------check what the registry entry for masking files *.doc or whatever is with universal shield

    UNICODE_STRING targetDirectory;

    NTSTATUS status = GetDirectoryName(Data, &targetDirectory);

    if (NT_SUCCESS(status))
    {
        DBG_PRINT("%wZ\n", &targetDirectory);
        UNICODE_STRING test;

        //status = RtlUnicodeStringInit(&test, L"\\Device\\HarddiskVolume2\\ProgramData\\VMware\\VMware Tools");
        status = RtlUnicodeStringInit(&test, L"\\Device\\HarddiskVolume2\\Users\\WDKRemoteUser.Win732Temp-PC.002\\Downloads\\hi\\New Text Document.txt");

        if (NT_SUCCESS(status))
        {
            LONG result = RtlCompareUnicodeString(&targetDirectory, &test, FALSE);

            if (result == 0)
            {
                DBG_PRINT("Hiding file %wZ\n", targetDirectory);
                Data->IoStatus.Status = STATUS_NO_SUCH_FILE;
                //return FLT_PREOP_COMPLETE;
            }
        }
    }

    //return FLT_PREOP_SUCCESS_NO_CALLBACK;
}

#endif


void AddNewFolder(FolderVector* folderVector)
{
    Folder folder;

    RtlUnicodeStringInit(&folder.VolumeDirPath, L"\\Device\\HarddiskVolume2\\ProgramData\\VMware\\VMware Tools");
    RtlUnicodeStringInit(&folder.PartitionDirPath, L"C:\\Users\\Administrator\\Desktop\\");

    AddNewFolderFiles(&folder);

    FolderVectorInsert(folderVector, folder);
}

void AddNewFolderFiles(Folder* folder)
{
    UnicodeStringVectorInitialize(&folder->FileNames, DEFAULT_VECTOR_CAPACITY);

    for (int i = 0; i < 12; i++)
    {
        UNICODE_STRING fileName;
        RtlUnicodeStringInit(&fileName, L"fileName.txt");

        UnicodeStringVectorInsert(&folder->FileNames, fileName);
    }
}

#ifdef __cplusplus
}
#endif