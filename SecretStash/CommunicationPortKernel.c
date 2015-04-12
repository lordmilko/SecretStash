#include "CommunicationPortKernel.h"
#include "../Shared/DataStructures.h"
#include "Exception.h"
#include "../Shared/CommunicationPortShared.h"

extern SecretStashConfig secretStashConfig;

NTSTATUS CreateCommunicationPort()
{
    PSECURITY_DESCRIPTOR securityDescriptor = NULL;

    NTSTATUS status = STATUS_SUCCESS;
    UNICODE_STRING portName;
    OBJECT_ATTRIBUTES attribs;

    try
    {
        LeaveOnFail(FltBuildDefaultSecurityDescriptor(&securityDescriptor, FLT_PORT_ALL_ACCESS));
        LeaveOnFail(RtlUnicodeStringInit(&portName, SECRETSTASH_PORT_NAME));

        InitializeObjectAttributes(&attribs, &portName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, securityDescriptor);

        LeaveOnFail(
            FltCreateCommunicationPort(
                secretStashConfig.Filter,
                &secretStashConfig.ComPortConfig.ServerPort,
                &attribs,
                NULL,
                SecretStashPortConnect,
                SecretStashPortDisconnect,
                SecretStashPortMessage,
                MAX_PORT_CONNECTIONS
            )
        );  
    }
    finally
    {
        if (securityDescriptor != NULL)
            FltFreeSecurityDescriptor(securityDescriptor);
    }

    return status;
}

NTSTATUS SecretStashPortConnect(PFLT_PORT ClientPort, PVOID ServerPortCookie, PVOID ConnectionContext, ULONG SizeOfContext, PVOID *ConnectionPortCookie)
{
    PAGED_CODE();

    UNREFERENCED_PARAMETER(ServerPortCookie);
    UNREFERENCED_PARAMETER(ConnectionContext);
    UNREFERENCED_PARAMETER(SizeOfContext);
    UNREFERENCED_PARAMETER(ConnectionPortCookie);

    FLT_ASSERT(secretStashConfig.ComPortConfig.ClientPort == NULL);

    secretStashConfig.ComPortConfig.ClientPort = ClientPort;

    return STATUS_SUCCESS;
}

void SecretStashPortDisconnect(PVOID ConnectionCookie)
{
    PAGED_CODE();

    UNREFERENCED_PARAMETER(ConnectionCookie);

    //Close our ClientPort
    FltCloseClientPort(secretStashConfig.Filter, &secretStashConfig.ComPortConfig.ClientPort);
}

NTSTATUS SecretStashPortMessage(PVOID PortCookie, PVOID InputBuffer, ULONG InputBufferLength, PVOID OutputBuffer, ULONG OutputBufferLength, PULONG ReturnOutputBufferLength)
{
    NTSTATUS status = STATUS_SUCCESS;

	if (InputBuffer != NULL && ValidInputBufferLength(InputBufferLength))
	{
		try
		{
            typedef struct _a {
                UNICODE_STRING b;
                UNICODE_STRING c;
            } a;

            a* ana = (a*)InputBuffer;
            ana;

			//UNICODE_STRING* str = (UNICODE_STRING*)InputBuffer;
			//str;

			int i = 0;
			i;
			//SecretStashCommand command = ((CommandMessage*)InputBuffer)->Command;
            //status = ProcessCommand(command);
		}
        except(ShouldHandleException(GetExceptionInformation(), TRUE))
		{
			return GetExceptionCode();
		}	
	}
    else
    {
        status = STATUS_INVALID_PARAMETER;
    }

    return status;
}

BOOLEAN ValidInputBufferLength(ULONG InputBufferLength)
{
	/*
	* The InputBuffer should always contain a CommandMessage - a data structure containing a "Command" attribute of type SecretStashCommand.
	*
	* In order to verify this requirement has been met, we must check the InputBuffer's Size against both the size of a CommandMessage, and the Command attribute's
	* size against the size of a SecretStashCommand. As we can't be sure we've received a valid CommandMessage however, it is not immediately safe to assume our InputBuffer
	* contains a Command attribute.
	*
	* These details can be ascertained indirectly by considering how big our InputBuffer WOULD be if a SecretStashCommand were contained anywhere within it - whether as the first attribute
	* of a CommandMessage or the last.
	*
	* Suppose a CommandMessage is 8 bytes, and a SecretStashCommand is 4 bytes, and a SecretStashCommand is the first field of a CommandMessage. This means that
	*
	*     InputBufferSize (8 bytes) >= FIELD(OFFSET(CommandMessage, Command) (0 bytes) + sizeof(SecretStashCommand) (4 bytes), or
	*     8 >= 0 + 4
	*     8 >= 4
	*
	* If the SecretStashCommand is the last field of the CommandMessage, there will be 4 bytes that come before it, giving us
	*
	*     InputBufferSize (8 bytes) >= FIELD(OFFSET(CommandMessage, Command) (4 bytes) + sizeof(SecretStashCommand) (4 bytes), or
	*     8 >= 4 + 4
	*     8 >= 8
	*
	* Regardless of where our SecretStashCommand is in our CommandMessage, our InputBufferSize is always equal to or greater than the size of the SecretStashCommand field offset plus the size of the SecretStashCommand's field itself
	*
	* Arguably, this whole calculation might seem needlessly complex (wouldn't just checking the InputBufferSize against the size of a CommandMessage suffice?) however the Minispy File System Filter Demo
	* uses this calculation, so there's probably a very good reason behind it - most likely to do with data structure alignments.
	*/

	BOOLEAN validLength = InputBufferLength >= (FIELD_OFFSET(CommandMessage, Command) + sizeof(SecretStashCommand));

	return validLength;
}

NTSTATUS ProcessCommand(SecretStashCommand command)
{
	switch (command)
	{
	case HideAllFiles:
		break;

	case HideSpecificFiles:
		break;

	case ShowAllFiles:
		break;

	case ShowSpecificFiles:
		break;

	default:
		break;
	}

    return 0;
}