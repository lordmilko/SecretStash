#include "MemoryManagement.h"

PVOID NTAPI ExReAllocatePoolWithTag(PVOID ExistingPool, POOL_TYPE PoolType, SIZE_T OldPoolSizeInBytes, SIZE_T NewPoolSizeInBytes, ULONG Tag)
{
    PVOID newPool = ExAllocatePoolWithTag(PoolType, NewPoolSizeInBytes, Tag);

    RtlCopyMemory(newPool, ExistingPool, OldPoolSizeInBytes);

    ExFreePoolWithTag(ExistingPool, Tag);

    return newPool;
}