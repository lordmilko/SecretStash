#pragma once

#include <fltKernel.h>
#include <ntstrsafe.h>
#include <dontuse.h>
#include <suppress.h>

#include "../Shared/Vector.h"

#pragma prefast(disable:__WARNING_ENCODE_MEMBER_FUNCTION_POINTER, "Not valid for kernel mode drivers")

#define __annotation

/*************************************************************************
    Prototypes
*************************************************************************/

DRIVER_INITIALIZE DriverEntry;

/*************************************************************************
MiniFilter initialization and unload routines.
*************************************************************************/

/*************************************************************************
Routine Description:
    This is the initialization routine for this miniFilter driver.  This registers with FltMgr and initializes all global data structures.

Arguments:
    DriverObject - Pointer to driver object created by the system to represent this driver.
    RegistryPath - Unicode string identifying where the parameters for this driver are located in the registry.

Return Value:
    Routine can return non success error codes.
*/
NTSTATUS DriverEntry (
    _In_ PDRIVER_OBJECT DriverObject,
    _In_ PUNICODE_STRING RegistryPath
);

NTSTATUS GetRegistryConfigData(PUNICODE_STRING RegistryPath);

/*************************************************************************
Routine Description:
    This is the unload routine for this miniFilter driver. This is called when the minifilter is about to be unloaded. We can fail this unload
    request if this is not a mandatory unload indicated by the Flags parameter.

Arguments:
    Flags - Indicating if this is a mandatory unload.

Return Value:
    Returns STATUS_SUCCESS.
*/
NTSTATUS SecretStashUnload (
    _In_ FLT_FILTER_UNLOAD_FLAGS Flags
);

/*************************************************************************
MiniFilter callback routines.
*************************************************************************/

/*************************************************************************
Routine Description:
    This routine is a pre-operation dispatch routine for this miniFilter.
    This is non-pageable because it could be called on the paging path

Arguments:
    Data - Pointer to the filter callbackData that is passed to us.
    FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing opaque handles to this filter, instance, its associated volume and file object.
    CompletionContext - The context for the completion routine for this operation.

Return Value:
    The return value is the status of the operation.
*/
FLT_PREOP_CALLBACK_STATUS SecretStashPreDirectoryControl (
    _Inout_ PFLT_CALLBACK_DATA Data,
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _Flt_CompletionContext_Outptr_ PVOID *CompletionContext
);

/*************************************************************************
Routine Description:
    This routine is the post-operation completion routine for this miniFilter.
    This is non-pageable because it may be called at DPC level.

Arguments:
    Data - Pointer to the filter callbackData that is passed to us.
    FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing opaque handles to this filter, instance, its associated volume and file object.
    CompletionContext - The completion context set in the pre-operation routine.
    Flags - Denotes whether the completion is successful or is being drained.

Return Value:
    The return value is the status of the operation.
*/
FLT_POSTOP_CALLBACK_STATUS SecretStashPostDirectoryControl (
    _Inout_ PFLT_CALLBACK_DATA Data,
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_opt_ PVOID CompletionContext,
    _In_ FLT_POST_OPERATION_FLAGS Flags
);

/*************************************************************************
    Global Variable Definitions
*************************************************************************/

/*
* Globals:
*     gFilterHandle - Handle to the minifilter driver
*     Callbacks - array of filesystem operations to filter
*     FilterRegistration - filter registration info
*     secretStashConfig - global config data
*/

//Array of filesystem operations to filter
CONST FLT_OPERATION_REGISTRATION Callbacks[] = {
    { IRP_MJ_DIRECTORY_CONTROL, 0, SecretStashPreDirectoryControl, SecretStashPostDirectoryControl },
    { IRP_MJ_OPERATION_END }
};

//Filter registration info
CONST FLT_REGISTRATION FilterRegistration = {

    sizeof(FLT_REGISTRATION), //  Size
    FLT_REGISTRATION_VERSION, //  Version
    0,                        //  Flags

    NULL,                     //  Context
    Callbacks,                //  Operation callbacks

    SecretStashUnload,        //  MiniFilterUnload

    NULL,                     //  InstanceSetup
    NULL,                     //  InstanceQueryTeardown
    NULL,                     //  InstanceTeardownStart
    NULL,                     //  InstanceTeardownComplete

    NULL,                     //  GenerateFileName
    NULL,                     //  GenerateDestinationFileName
    NULL                      //  NormalizeNameComponent
};

SecretStashConfig secretStashConfig;

/*************************************************************************
    Assign text sections for each routine.
*************************************************************************/

#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(PAGE, SecretStashUnload)
#pragma alloc_text(PAGE, SecretStashPreDirectoryControl)
#pragma alloc_text(PAGE, SecretStashPostDirectoryControl)
#endif