#pragma once

#include "DataStructures.h"

#ifdef __cplusplus
namespace SecretStash
{
#endif

#define DEFAULT_VECTOR_CAPACITY 10

void FolderVectorInitialize(FolderVector* vector, size_t capacity);
void FolderVectorInsert(FolderVector* vector, Folder value);
void FolderVectorResize(FolderVector* vector);
Folder* FolderVectorGet(FolderVector* vector, size_t index);
void FolderVectorRemove(FolderVector* vector);

void UnicodeStringVectorInitialize(UnicodeStringVector* vector, size_t capacity);
void UnicodeStringVectorInsert(UnicodeStringVector* vector, UNICODE_STRING value);
void UnicodeStringVectorResize(UnicodeStringVector* vector);
UNICODE_STRING* UnicodeStringVectorGet(UnicodeStringVector* vector, size_t index);
void UnicodeStringVectorRemove(UnicodeStringVector* vector);

#ifdef __cplusplus
} /* namespace SecretStash */
#endif