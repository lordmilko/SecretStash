#include "stdafx.h"
#include "CommunicationPortUser.h"

#include "../Shared/CommunicationPortShared.h"
#include <fltUser.h>
#pragma comment(lib, "FltLib.lib")

#include "UnicodeString.h"

namespace SecretStash
{
	void testcommunication()
	{
		HANDLE port = INVALID_HANDLE_VALUE;

        UNICODE_STRING test1;
        RtlUnicodeStringInit(&test1, L"C:\\Program Files\\Microsoft\\filename.txt");

        UNICODE_STRING folder;
        UNICODE_STRING file;

        NTSTATUS status = GetFolderAndFileFromFilePath(&test1, &folder, &file);
        status;

        typedef struct _a {
            UNICODE_STRING b;
            UNICODE_STRING c;
        } a;

        //maybe we could just provide raw memory access to the items selected in the listview?

        a ana;
        ana.b = folder;
        ana.c = file;

		HRESULT result = FilterConnectCommunicationPort(SECRETSTASH_PORT_NAME, NULL, NULL, NULL, NULL, &port);

		if (result == S_OK)
		{
			//CommandMessage message;
			//message.Command = HideAllFiles;
			UNICODE_STRING test;
			RtlUnicodeStringInit(&test, L"test");

			//LPVOID buffer[4096 / sizeof(LPVOID)];
			DWORD bytesReturned = 0;

			//use this to send data http://stackoverflow.com/questions/10986551/windows-driver-passing-strings-between-user-mode-and-kernel-mode-dynamically

			result = FilterSendMessage(
				port,		//Port
                &ana,		//Buffer containing the message to be sent
                sizeof(ana),//Size of the buffer above
				NULL,		//Buffer that receives the reply
				NULL,		//Size of the buffer above
				&bytesReturned		//Amount of data that was actually written to the buffer above
				);

			CloseHandle(port);
		}

        free(folder.Buffer);
        free(file.Buffer);
	}

}