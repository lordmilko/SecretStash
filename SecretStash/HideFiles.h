#pragma once

#include <fltKernel.h>
#include <ntstrsafe.h>
#include <dontuse.h>
#include <suppress.h>

#include "../Shared/DataStructures.h"

BOOLEAN ShouldHideFiles(PFLT_CALLBACK_DATA Data);
BOOLEAN FolderContainsFilesToHide(PFLT_CALLBACK_DATA Data);
NTSTATUS GetDirectoryName(PFLT_CALLBACK_DATA Data, PUNICODE_STRING directoryName);
BOOLEAN IsRelevantFileInfoQuery(PFLT_CALLBACK_DATA Data);

void ProcessFileInfoQuery(PFLT_CALLBACK_DATA Data);

BOOLEAN ShouldHideFile(UNICODE_STRING filename, PFLT_CALLBACK_DATA Data);
BOOLEAN CheckFileToHideAgainstFolderVector(UNICODE_STRING* directory, UNICODE_STRING* filename);
void CheckFileToHideAgainstFolder(Folder* folder, UNICODE_STRING* directory, UNICODE_STRING* filename, BOOLEAN* hideFile);
void CheckFileToHideAgainstFolderFileNames(Folder* folder, UNICODE_STRING* directory, UNICODE_STRING* filename, BOOLEAN* hideFile);