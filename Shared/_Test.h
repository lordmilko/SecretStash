#pragma once

#ifdef KERNEL_MODE
#include <fltKernel.h>
#include <ntstrsafe.h>
#include <dontuse.h>
#include <suppress.h>
#endif

#include "../Shared/DataStructures.h"

#ifdef __cplusplus
namespace SecretStash
{
#endif

void InsertTemporaryRegData(PUNICODE_STRING RegistryPath);
void AddNewFolder(FolderVector* folderVector);
void AddNewFolderFiles(Folder* folder);
void PrintFolderValues(FolderVector* folderVector);

#ifdef __cplusplus
}
#endif