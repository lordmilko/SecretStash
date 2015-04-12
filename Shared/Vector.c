#include "Vector.h"

#define __annotation

#ifdef KERNEL_MODE

#include "MemoryManagement.h"
#define SS_ASSERT NT_ASSERT

#else

#include "../SecretStashUI/UnicodeString.h"

#include <malloc.h>
#include <crtdbg.h>
#define SS_ASSERT(expr) _ASSERT_EXPR((expr), _CRT_WIDE(#expr))
#define FALSE 0

#endif

//#define NT_ASSERT

#ifdef __cplusplus
namespace SecretStash
{
#endif

/*************************************************************************
    Folder Vector
 *************************************************************************/

void FolderVectorInitialize(FolderVector* vector, size_t capacity)
{
    vector->size = 0;
    vector->elementSize = sizeof(Folder);
    vector->capacity = capacity;

#if KERNEL_MODE
    vector->data = (Folder*)ExAllocatePoolWithTag(PagedPool, vector->elementSize * vector->capacity, VECTOR_DATA_TAG);
#else
    vector->data = (Folder*)malloc(vector->elementSize * vector->capacity);
#endif

    SS_ASSERT(vector->data != NULL); //need to handle this better. maybe write to dbgprint and unload? how does one safely decide to unload ones self
}

void FolderVectorInsert(FolderVector* vector, Folder value)
{
    if (vector->size == vector->capacity)
    {
        FolderVectorResize(vector);
    }

    vector->data[vector->size] = value;
    vector->size++;
}

void FolderVectorResize(FolderVector* vector)
{
    size_t newCapacity = 2 * vector->capacity;

#if KERNEL_MODE
    size_t oldCapacityInBytes = vector->capacity * vector->elementSize;
    size_t newCapacityInBytes = newCapacity * vector->elementSize;
    Folder* temp = (Folder*)ExReAllocatePoolWithTag(vector->data, PagedPool, oldCapacityInBytes, newCapacityInBytes, VECTOR_DATA_TAG);
#else
    Folder* temp = (Folder*)realloc(vector->data, newCapacity);
#endif

    if (temp != NULL)
    {
        vector->data = temp;
        vector->capacity = newCapacity;
    }
    else
        SS_ASSERT(FALSE); //--------------------------------------------------------------------------------HANDLE THIS SOMEHOW INSTEAD OF ASSERTING
}

Folder* FolderVectorGet(FolderVector* vector, size_t index)
{
    if (index <= vector->size)
        return &vector->data[index];
    else
        return NULL;
}

void FolderVectorRemove(FolderVector* vector)
{
    if (vector->data != NULL)
    {
#if KERNEL_MODE
        ExFreePoolWithTag(vector->data, VECTOR_DATA_TAG);
#else
        free(vector->data);
#endif
    }

}

/*************************************************************************
    UNICODE_STRING Vector
    *************************************************************************/

void UnicodeStringVectorInitialize(UnicodeStringVector* vector, size_t capacity)
{
    vector->size = 0;
    vector->elementSize = sizeof(UNICODE_STRING);
    vector->capacity = capacity;

#if KERNEL_MODE
    vector->data = (UNICODE_STRING*)ExAllocatePoolWithTag(PagedPool, vector->elementSize * vector->capacity, VECTOR_DATA_TAG);
#else
    vector->data = (UNICODE_STRING*)malloc(vector->elementSize * vector->capacity);
#endif

    SS_ASSERT(vector->data != NULL); //need to handle this better. maybe write to dbgprint and unload? how does one safely decide to unload ones self
}

void UnicodeStringVectorInsert(UnicodeStringVector* vector, UNICODE_STRING value)
{
    if (vector->size == vector->capacity)
    {
        UnicodeStringVectorResize(vector);
    }

    vector->data[vector->size] = value; //on the 10th insertion everything got messed up. realloc failed, hard?
    vector->size++;
}

void UnicodeStringVectorResize(UnicodeStringVector* vector)
{
    size_t newCapacity = 2 * vector->capacity;
    size_t newCapacityInBytes = newCapacity * vector->elementSize;
#if KERNEL_MODE
    size_t oldCapacityInBytes = vector->capacity * vector->elementSize;
    UNICODE_STRING* temp = (UNICODE_STRING*)ExReAllocatePoolWithTag(vector->data, PagedPool, oldCapacityInBytes, newCapacityInBytes, VECTOR_DATA_TAG);
#else
    UNICODE_STRING* temp = (UNICODE_STRING*)realloc(vector->data, newCapacityInBytes); //compare the address this returns with what vector->data currently is
#endif

    if (temp != NULL)
    {
        vector->data = temp;
        vector->capacity = newCapacity;
    }
    else
        SS_ASSERT(FALSE); //--------------------------------------------------------------------------------HANDLE THIS SOMEHOW INSTEAD OF ASSERTING
}

UNICODE_STRING* UnicodeStringVectorGet(UnicodeStringVector* vector, size_t index)
{
    if (index <= vector->size)
        return &vector->data[index];
    else
        return NULL;
}

void UnicodeStringVectorRemove(UnicodeStringVector* vector)
{
    if (vector->data != NULL)
    {
#if KERNEL_MODE
        ExFreePoolWithTag(vector->data, VECTOR_DATA_TAG);
#else
        free(vector->data);
#endif
    }

}

#ifdef __cplusplus
} /* namespace SecretStash */
#endif