/*

Module Name:
    SecretStash.c

Abstract:
    This is the main module of the SecretStash miniFilter driver.

Overview:
    SecretStash allows users to hide files and folders from directory queries against the filesystem.
    Any item hidden by SecretStash is completely accessible to anyone who knows of the item's existance,
    or who otherwise has a direct file path. Configuration of SecretStash is performed by SecretStashUI,
    a UI frontend included with this driver.

Environment:
    Kernel mode

*/

#include "SecretStash.h"
#include "Debug.h"
#include "Registry.h"
#include "HideFiles.h"
#include "CommunicationPortKernel.h"
#include "Exception.h"
#include "../Shared/_Test.h"

#pragma comment(lib, "ntstrsafe.lib")

//-------------------------------------------------------------------------------------------------when done, need to check alignment on all our structs for x32 AND x64

/*************************************************************************
    MiniFilter initialization and unload routines.
*************************************************************************/


NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
    DBG_PRINT("SecretStash!DriverEntry: Entered\n");

    secretStashConfig.RegDataLoaded = FALSE;
    NTSTATUS status = STATUS_SUCCESS;
    try
    {
        LeaveOnFail(FltRegisterFilter(DriverObject, &FilterRegistration, &secretStashConfig.Filter)); //  Register with FltMgr to tell it our callback routines
        LeaveOnFail(GetRegistryConfigData(RegistryPath));
        LeaveOnFail(CreateCommunicationPort());
        LeaveOnFail(FltStartFiltering(secretStashConfig.Filter));
    }
    finally
    {
        if (!NT_SUCCESS(status))
        {
            if (secretStashConfig.ComPortConfig.ServerPort != NULL)
                FltCloseCommunicationPort(secretStashConfig.ComPortConfig.ServerPort);

            if (secretStashConfig.Filter != NULL)
                FltUnregisterFilter(secretStashConfig.Filter);
        }
    }

    return status;
}

NTSTATUS GetRegistryConfigData(PUNICODE_STRING RegistryPath)
{
    InsertTemporaryRegData(RegistryPath); //---------------------------------------------------------------------------------------DELETE

    NTSTATUS status = QueryRegistryValue(RegistryPath, L"Data", ConfigDataQueryRoutine);

    return status;
}

NTSTATUS SecretStashUnload(FLT_FILTER_UNLOAD_FLAGS Flags)
{
    UNREFERENCED_PARAMETER(Flags);

    PAGED_CODE();

    DBG_PRINT("SecretStash!SecretStashUnload: Entered\n");

    FltCloseCommunicationPort(secretStashConfig.ComPortConfig.ServerPort);

    if (secretStashConfig.RegDataLoaded)
        DestroyFolderVector(&secretStashConfig.FolderVector);

    FltUnregisterFilter(secretStashConfig.Filter);

    return STATUS_SUCCESS;
}

/*************************************************************************
    MiniFilter callback routines.
*************************************************************************/

FLT_PREOP_CALLBACK_STATUS SecretStashPreDirectoryControl(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID* CompletionContext)
{
    UNREFERENCED_PARAMETER(FltObjects);
    UNREFERENCED_PARAMETER(CompletionContext);

    if (ShouldHideFiles(Data))
    {
        /* If this folder contains files that need hiding, we won't be able to hide them until the folders files are retrieved from the filesystem.
         * We signify we'd like to callback to our PostOp function, so when the IRP returns we can modify the folders file list as desired */
        return FLT_PREOP_SUCCESS_WITH_CALLBACK;
    }
    
    return FLT_PREOP_SUCCESS_NO_CALLBACK;
}

FLT_POSTOP_CALLBACK_STATUS SecretStashPostDirectoryControl(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID CompletionContext, FLT_POST_OPERATION_FLAGS Flags)
{
    UNREFERENCED_PARAMETER(FltObjects);
    UNREFERENCED_PARAMETER(CompletionContext);
    UNREFERENCED_PARAMETER(Flags);

    if (NT_SUCCESS(Data->IoStatus.Status))
    {
        ProcessFileInfoQuery(Data);
    }

    return FLT_POSTOP_FINISHED_PROCESSING;
}