#pragma once

#include <fltKernel.h>
#include <ntstrsafe.h>
#include "../Shared/DataStructures.h"
#include <dontuse.h>
#include <suppress.h>

NTSTATUS QueryRegistryValue(PUNICODE_STRING RegistryPath, PWSTR ValueName, PRTL_QUERY_REGISTRY_ROUTINE queryRoutine);

RTL_QUERY_REGISTRY_ROUTINE ConfigDataQueryRoutine;
NTSTATUS ConfigDataQueryRoutine(PWSTR ValueName, ULONG ValueType, PVOID ValueData, ULONG ValueLength, PVOID Context, PVOID EntryContext);

NTSTATUS WriteRegistryValue(PUNICODE_STRING RegistryPath, PWSTR ValueName, ULONG ValueType, ByteBuffer byteBuffer);