#include "Registry.h"
#include "Debug.h"
#include "../Shared/Decode.h"

extern SecretStashConfig secretStashConfig;

NTSTATUS QueryRegistryValue(PUNICODE_STRING RegistryPath, PWSTR ValueName, PRTL_QUERY_REGISTRY_ROUTINE queryRoutine)
{
    //queryTable must end in an entry with a NULL Name and QueryRoutine field
    RTL_QUERY_REGISTRY_TABLE queryTable[2];

    RtlZeroBytes(&queryTable[0], sizeof(queryTable));

    queryTable[0].QueryRoutine = queryRoutine;
    queryTable[0].Flags = RTL_QUERY_REGISTRY_REQUIRED;
    queryTable[0].Name = ValueName;
    queryTable[0].EntryContext = NULL;
    queryTable[0].DefaultType = REG_NONE;
    queryTable[0].DefaultData = NULL;
    queryTable[0].DefaultLength = 0;

    NTSTATUS status = RtlQueryRegistryValues(RTL_REGISTRY_ABSOLUTE, RegistryPath->Buffer, &queryTable[0], NULL, NULL);
    
    return status;
}

NTSTATUS ConfigDataQueryRoutine(PWSTR ValueName, ULONG ValueType, PVOID ValueData, ULONG ValueLength, PVOID Context, PVOID EntryContext)
{
    UNREFERENCED_PARAMETER(ValueName);
    UNREFERENCED_PARAMETER(ValueType);
    UNREFERENCED_PARAMETER(Context);
    UNREFERENCED_PARAMETER(EntryContext);

    ByteBuffer byteBuffer;
    byteBuffer.Buffer = (BYTE*)ValueData;
    byteBuffer.Position = 0;
    byteBuffer.Size = ValueLength;

    secretStashConfig.FolderVector = GetFolderVectorFromBytes(&byteBuffer);
    secretStashConfig.RegDataLoaded = TRUE;

    DBG_PRINT("SecretStash!ConfigDataQueryRoutine: Successfully retrieved FolderVector Config Data from the Registry\n");

    return STATUS_SUCCESS;
}

NTSTATUS WriteRegistryValue(PUNICODE_STRING RegistryPath, PWSTR ValueName, ULONG ValueType, ByteBuffer byteBuffer)
{
    NTSTATUS status = RtlWriteRegistryValue(RTL_REGISTRY_ABSOLUTE, RegistryPath->Buffer, ValueName, ValueType, byteBuffer.Buffer, byteBuffer.Size);

    return status;
}