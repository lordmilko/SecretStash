#pragma once

#ifdef KERNEL_MODE
#include <fltKernel.h>
#include <dontuse.h>
#include <suppress.h>
#endif

#ifdef __cplusplus
namespace SecretStash
{
#endif

#define MAX_PORT_CONNECTIONS 1
#define SECRETSTASH_PORT_NAME L"\\SecretStashPort"

typedef enum _SecretStashCommand {
    HideAllFiles,
    ShowAllFiles,
    HideSpecificFiles,
    ShowSpecificFiles
} SecretStashCommand;

typedef struct _CommandMessage {
	SecretStashCommand Command;
    PVOID Data;
    ULONG DataLength;
} CommandMessage;

#ifdef __cplusplus
}
#endif