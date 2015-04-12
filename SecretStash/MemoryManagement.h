#pragma once

#include <fltKernel.h>
#include <dontuse.h>
#include <suppress.h>

#define VECTOR_DATA_TAG 'SSVD'
#define UNICODE_STRING_TAG 'SSUS'
#define BYTE_BUFFER_TAG 'SSBB'

PVOID NTAPI ExReAllocatePoolWithTag(PVOID ExistingPool, POOL_TYPE PoolType, SIZE_T OldPoolSizeInBytes, SIZE_T NewPoolSizeInBytes, ULONG Tag);