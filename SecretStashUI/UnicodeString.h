#pragma once

#include <minwindef.h>

typedef struct _LSA_UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PWSTR  Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef const UNICODE_STRING *PCUNICODE_STRING;

typedef const wchar_t* NTSTRSAFE_PCWSTR;
typedef LONG NTSTATUS;
typedef const wchar_t* STRSAFE_PCNZWCH;

#define NTSTRSAFEDDI      __inline NTSTATUS __stdcall
#define NTSTRSAFEWORKERDDI    static NTSTRSAFEDDI

#define NTSTRSAFE_UNICODE_STRING_MAX_CCH    (0xffff / sizeof(wchar_t))  // max buffer size, in characters, for a UNICODE_STRING
#define STATUS_SUCCESS                   ((NTSTATUS)0x00000000L)
#define STATUS_BUFFER_OVERFLOW           ((NTSTATUS)0x80000005L)
#define STRSAFE_IGNORE_NULLS                        0x00000100

#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)

NTSTRSAFEDDI RtlUnicodeStringInit(PUNICODE_STRING DestinationString, NTSTRSAFE_PCWSTR pszSrc);
NTSTRSAFEWORKERDDI RtlUnicodeStringInitWorker(PUNICODE_STRING DestinationString, NTSTRSAFE_PCWSTR pszSrc, const size_t cchMax, DWORD dwFlags);
NTSTRSAFEWORKERDDI RtlStringLengthWorkerW(STRSAFE_PCNZWCH psz, size_t cchMax, size_t* pcchLength);

NTSTRSAFEDDI RtlUnicodeStringCopy(PUNICODE_STRING DestinationString, PCUNICODE_STRING SourceString);
NTSTRSAFEWORKERDDI RtlUnicodeStringValidateDestWorker(PCUNICODE_STRING DestinationString, wchar_t** ppszDest, size_t* pcchDest, size_t* pcchDestLength, const size_t cchMax, DWORD dwFlags);
NTSTRSAFEWORKERDDI RtlUnicodeStringValidateSrcWorker(PCUNICODE_STRING SourceString, wchar_t** ppszSrc, size_t* pcchSrcLength, const size_t cchMax, DWORD dwFlags);
NTSTRSAFEWORKERDDI RtlUnicodeStringValidateWorker(PCUNICODE_STRING SourceString, const size_t cchMax, DWORD dwFlags);
NTSTRSAFEWORKERDDI RtlWideCharArrayCopyWorker(wchar_t* pszDest, size_t cchDest, size_t* pcchNewDestLength, const wchar_t* pszSrc, size_t cchSrcLength);

NTSTATUS NTAPI RtlAppendUnicodeStringToString(PUNICODE_STRING Destination, PCUNICODE_STRING Source);

/* Initialize Unicode String */

NTSTRSAFEDDI RtlUnicodeStringInit(PUNICODE_STRING DestinationString, NTSTRSAFE_PCWSTR pszSrc)
{
	return RtlUnicodeStringInitWorker(DestinationString, pszSrc, NTSTRSAFE_UNICODE_STRING_MAX_CCH, 0);
}

NTSTRSAFEWORKERDDI RtlUnicodeStringInitWorker(PUNICODE_STRING DestinationString, NTSTRSAFE_PCWSTR pszSrc, const size_t cchMax, DWORD dwFlags)
{
	NTSTATUS status = STATUS_SUCCESS;

	if (DestinationString || !(dwFlags & STRSAFE_IGNORE_NULLS))
	{
		DestinationString->Length = 0;
		DestinationString->MaximumLength = 0;
		DestinationString->Buffer = NULL;
	}

	if (pszSrc)
	{
		size_t cchSrcLength;

		status = RtlStringLengthWorkerW(pszSrc, cchMax, &cchSrcLength);

		if (NT_SUCCESS(status))
		{
			if (DestinationString)
			{
				size_t cbLength;

				// safe to multiply cchSrcLength * sizeof(wchar_t) since cchSrcLength < NTSTRSAFE_UNICODE_STRING_MAX_CCH and sizeof(wchar_t) is 2
				cbLength = cchSrcLength * sizeof(wchar_t);

				DestinationString->Length = (USHORT)cbLength;
				// safe to add cbLength + sizeof(wchar_t) since cchSrcLength < NTSTRSAFE_UNICODE_STRING_MAX_CCH
				DestinationString->MaximumLength = (USHORT)(cbLength + sizeof(wchar_t));
				DestinationString->Buffer = (PWSTR)pszSrc;
			}
			else
			{
				status = STATUS_INVALID_PARAMETER;
			}
		}
	}

	return status;
}

NTSTRSAFEWORKERDDI RtlStringLengthWorkerW(STRSAFE_PCNZWCH psz, size_t cchMax, size_t* pcchLength)
{
	NTSTATUS status = STATUS_SUCCESS;
	size_t cchOriginalMax = cchMax;

	while (cchMax && (*psz != L'\0'))
	{
		psz++;
		cchMax--;
	}

	if (cchMax == 0)
	{
		// the string is longer than cchMax
		status = STATUS_INVALID_PARAMETER;
	}

	if (pcchLength)
	{
		if (NT_SUCCESS(status))
		{
			*pcchLength = cchOriginalMax - cchMax;
		}
		else
		{
			*pcchLength = 0;
		}
	}

	return status;
}

/* Copy Unicode String */

NTSTRSAFEDDI RtlUnicodeStringCopy(PUNICODE_STRING DestinationString, PCUNICODE_STRING SourceString)
{
	NTSTATUS status;
	wchar_t* pszDest; //The buffer of the UNICODE_STRING
	size_t cchDest; //The maximum number of characters the UNICODE_STRING can hold

	status = RtlUnicodeStringValidateDestWorker(DestinationString, &pszDest, &cchDest, NULL, NTSTRSAFE_UNICODE_STRING_MAX_CCH, 0);

	if (NT_SUCCESS(status))
	{
		wchar_t* pszSrc; //The buffer of the UNICODE_STRING
		size_t cchSrcLength; //The current number of characters in the UNICODE_STRING
		size_t cchNewDestLength = 0;

		status = RtlUnicodeStringValidateSrcWorker(SourceString, &pszSrc, &cchSrcLength, NTSTRSAFE_UNICODE_STRING_MAX_CCH, 0);

		if (NT_SUCCESS(status))
		{
            //dest buffer, dest max num chars, pointer for storing dest's length, source buffer, num characters in the source
			status = RtlWideCharArrayCopyWorker(pszDest, cchDest, &cchNewDestLength, pszSrc, cchSrcLength);
		}

		// safe to multiply cchNewDestLength * sizeof(wchar_t) since cchDest < NTSTRSAFE_UNICODE_STRING_MAX_CCH and sizeof(wchar_t) is 2
		DestinationString->Length = (USHORT)(cchNewDestLength * sizeof(wchar_t));
	}

	return status;
}

NTSTRSAFEWORKERDDI RtlUnicodeStringValidateDestWorker(PCUNICODE_STRING DestinationString, wchar_t** ppszDest, size_t* pcchDest, size_t* pcchDestLength, const size_t cchMax, DWORD dwFlags)
{
	NTSTATUS status;

	*ppszDest = NULL;
	*pcchDest = 0;

	if (pcchDestLength)
	{
		*pcchDestLength = 0;
	}

	status = RtlUnicodeStringValidateWorker(DestinationString, cchMax, dwFlags);

	if (NT_SUCCESS(status) && DestinationString)
	{
		*ppszDest = DestinationString->Buffer;
		*pcchDest = DestinationString->MaximumLength / sizeof(wchar_t);

		if (pcchDestLength)
		{
			*pcchDestLength = DestinationString->Length / sizeof(wchar_t);
		}
	}

	return status;
}

NTSTRSAFEWORKERDDI RtlUnicodeStringValidateSrcWorker(PCUNICODE_STRING SourceString, wchar_t** ppszSrc, size_t* pcchSrcLength, const size_t cchMax, DWORD dwFlags)
{
	NTSTATUS status;

	*ppszSrc = NULL;
	*pcchSrcLength = 0;

	status = RtlUnicodeStringValidateWorker(SourceString, cchMax, dwFlags);

	if (NT_SUCCESS(status))
	{
		if (SourceString)
		{
			*ppszSrc = SourceString->Buffer;
			*pcchSrcLength = SourceString->Length / sizeof(wchar_t);
		}

		if ((*ppszSrc == NULL) && (dwFlags & STRSAFE_IGNORE_NULLS))
		{
			*ppszSrc = L"";
		}
	}

	return status;
}

NTSTRSAFEWORKERDDI RtlUnicodeStringValidateWorker(PCUNICODE_STRING SourceString, const size_t cchMax, DWORD dwFlags)
{
	NTSTATUS status = STATUS_SUCCESS;

	if (SourceString || !(dwFlags & STRSAFE_IGNORE_NULLS))
	{
		if (((SourceString->Length % sizeof(wchar_t)) != 0) ||
			((SourceString->MaximumLength % sizeof(wchar_t)) != 0) ||
			(SourceString->Length > SourceString->MaximumLength) ||
			(SourceString->MaximumLength > (cchMax * sizeof(wchar_t))))
		{
			status = STATUS_INVALID_PARAMETER;
		}
		else if ((SourceString->Buffer == NULL) &&
			((SourceString->Length != 0) || (SourceString->MaximumLength != 0)))
		{
			status = STATUS_INVALID_PARAMETER;
		}
	}

	return status;
}

//Destination buffer, number of characters that can fit in the destination, counter for the destinations length, source buffer, number of characters in the source buffer
NTSTRSAFEWORKERDDI RtlWideCharArrayCopyWorker(wchar_t* pszDest, size_t cchDest, size_t* pcchNewDestLength, const wchar_t* pszSrc, size_t cchSrcLength)
{
	NTSTATUS status = STATUS_SUCCESS;
	size_t cchNewDestLength = 0;

    //while we can still fit characters in the destination and there are still characters in the source to loop through
	while (cchDest && cchSrcLength)
	{
		*pszDest++ = *pszSrc++;
		cchDest--;
		cchSrcLength--;

		cchNewDestLength++;
	}

    //we WOULD have had a buffer overflow, had we not been checking for when either one of these is 0
	if ((cchDest == 0) && (cchSrcLength != 0))
	{
		// we are going to truncate pszDest
		status = STATUS_BUFFER_OVERFLOW;
	}

	*pcchNewDestLength = cchNewDestLength;

	return status;
}

/* Append Unicode String */

#define STATUS_BUFFER_TOO_SMALL   ((NTSTATUS)0xC0000023)

NTSTRSAFEDDI RtlAppendUnicodeStringToString(PUNICODE_STRING Destination, PCUNICODE_STRING Source)
{
	USHORT SourceLength = Source->Length;
	PWCHAR Buffer = &Destination->Buffer[Destination->Length / sizeof(WCHAR)];

	if (SourceLength)
	{
		if ((SourceLength + Destination->Length) > Destination->MaximumLength)
		{
			return STATUS_BUFFER_TOO_SMALL;
		}

		RtlMoveMemory(Buffer, Source->Buffer, SourceLength);
		Destination->Length += SourceLength;

		/* append terminating '\0' if enough space */
		if (Destination->MaximumLength > Destination->Length)
		{
			Buffer[SourceLength / sizeof(WCHAR)] = UNICODE_NULL;
		}
	}

	return STATUS_SUCCESS;
}

//-----------------------------------------------------

#define STATUS_NOT_FOUND                 ((NTSTATUS)0xC0000225L)

//------------------TEST THIS WORKS - AND IT'S SAFE!!!! then, using the index of the character, get do index*sizeof(wchar) to get the offset in the buffer and then copy from buffer to (buffer+index*sizeof(wchar)
//and from buffer+index*sizeof(wchar) to unicode_string.length

NTSTRSAFEDDI UnicodeStringLastIndexOfChar(PUNICODE_STRING SourceString, WCHAR Character, USHORT* CharacterIndex)
{
    NTSTATUS status = STATUS_NOT_FOUND;

    WCHAR* buffer = SourceString->Buffer;

    USHORT numChars = SourceString->Length / sizeof(WCHAR);

    for (USHORT i = numChars; i-- > 0;)
    {
        WCHAR character = buffer[i];

        if (character == Character)
        {
            *CharacterIndex = i;
            status = STATUS_SUCCESS;
            break;
        } 
    }

    return status;
}
#include <malloc.h>
NTSTRSAFEDDI UnicodeStringSubString(PUNICODE_STRING SourceString, USHORT IndexFrom, USHORT CharsToCopy, PUNICODE_STRING DestinationString)
{
    //USHORT bytesToCopy = charsToCopy * sizeof(WCHAR);

    //DestinationString->Buffer = (PWSTR)malloc(bytesToCopy + sizeof(WCHAR));

    wchar_t* destinationBuffer = DestinationString->Buffer; //WE SHOULD PROBABLY ALLOCATE THIS
    //DestinationString->MaximumLength = bytesToCopy + sizeof(WCHAR);
    size_t newDestLength = 0;

    wchar_t* sourceBuffer = SourceString->Buffer + IndexFrom;

    NTSTATUS status = RtlWideCharArrayCopyWorker(destinationBuffer, CharsToCopy, &newDestLength, sourceBuffer, CharsToCopy);

    DestinationString->Length = (USHORT)newDestLength * sizeof(WCHAR);

    return status;
}

NTSTRSAFEDDI GetFolderNameFromFilePath(PUNICODE_STRING FilePath, PUNICODE_STRING FolderName, USHORT IndexOfLastSlash)
{
    USHORT charsToCopy = IndexOfLastSlash;
    USHORT bytesToCopy = charsToCopy * sizeof(WCHAR);

    FolderName->MaximumLength = bytesToCopy + sizeof(WCHAR);
    FolderName->Buffer = (PWSTR)malloc(FolderName->MaximumLength);
    FolderName->Length = 0;

    NTSTATUS status = UnicodeStringSubString(FilePath, 0, charsToCopy, FolderName);

    return status;
}

NTSTRSAFEDDI GetFileNameFromFilePath(PUNICODE_STRING FilePath, PUNICODE_STRING FileName, USHORT IndexOfLastSlash)
{
    USHORT charsToCopy = (FilePath->Length / sizeof(WCHAR)) - (IndexOfLastSlash + 1);
    USHORT bytesToCopy = charsToCopy * sizeof(WCHAR);

    FileName->MaximumLength = bytesToCopy + sizeof(WCHAR);
    FileName->Buffer = (PWSTR)malloc(FileName->MaximumLength);
    FileName->Length = 0;

    NTSTATUS status = UnicodeStringSubString(FilePath, IndexOfLastSlash + 1, charsToCopy, FileName);

    return status;
}

NTSTRSAFEDDI GetFolderAndFileFromFilePath(PUNICODE_STRING FilePath, PUNICODE_STRING FolderName, PUNICODE_STRING FileName)
{
    USHORT indexOfLastSlash = 0;

    NTSTATUS status = UnicodeStringLastIndexOfChar(FilePath, L'\\', &indexOfLastSlash);

    if (NT_SUCCESS(status))
    {
        status = GetFolderNameFromFilePath(FilePath, FolderName, indexOfLastSlash);

        if (NT_SUCCESS(status))
        {
            status = GetFileNameFromFilePath(FilePath, FileName, indexOfLastSlash);
        }
    }

    return status;
}
//NTSTRSAFEDDI RtlSplitUnicodeString(PUNICODE_STRING SourceString, WCHAR SplitChar, PUNICODE_STRING )