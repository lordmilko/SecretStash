#pragma once

#include <fltKernel.h>
#include <ntstrsafe.h>
#include <dontuse.h>
#include <suppress.h>

#include "../Shared/CommunicationPortShared.h"

#define __annotation

NTSTATUS CreateCommunicationPort();

NTSTATUS SecretStashPortConnect(PFLT_PORT ClientPort, PVOID ServerPortCookie, PVOID ConnectionContext, ULONG SizeOfContext, PVOID *ConnectionPortCookie);
void SecretStashPortDisconnect(PVOID ConnectionCookie);
NTSTATUS SecretStashPortMessage(PVOID PortCookie, PVOID InputBuffer, ULONG InputBufferLength, PVOID OutputBuffer, ULONG OutputBufferLength, PULONG ReturnOutputBufferLength);
BOOLEAN ValidInputBufferLength(ULONG InputBufferLength);
NTSTATUS ProcessCommand(SecretStashCommand command);