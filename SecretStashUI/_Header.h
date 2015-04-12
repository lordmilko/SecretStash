#pragma once

//whatever we do, we dont just want to include windows.h. we want it to detect stdafx. does including this file once somewhere sensible after a stdafx include work?

typedef struct _LSA_UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PWSTR  Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef _Return_type_success_(return >= 0) LONG NTSTATUS;

typedef const wchar_t* NTSTRSAFE_PCWSTR;
#define NTSTRSAFE_UNICODE_STRING_MAX_CCH    (0xffff / sizeof(wchar_t))  // max buffer size, in characters, for a UNICODE_STRING
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
#define STATUS_SUCCESS                   ((NTSTATUS)0x00000000L)    // ntsubauth
#define STRSAFE_IGNORE_NULLS                            0x00000100  // treat null string pointers as TEXT("") -- don't fault on NULL buffers
typedef  const wchar_t* STRSAFE_PCNZWCH;


#define NTSTRSAFEDDI      __inline NTSTATUS __stdcall
#define NTSTRSAFEWORKERDDI    static NTSTRSAFEDDI
NTSTRSAFEWORKERDDI
RtlStringLengthWorkerW(
_In_reads_or_z_(cchMax) STRSAFE_PCNZWCH psz,
_In_ _In_range_(<= , NTSTRSAFE_MAX_CCH) size_t cchMax,
_Out_opt_ _Deref_out_range_(<, cchMax) _Deref_out_range_(<= , _String_length_(psz)) size_t* pcchLength)
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

NTSTRSAFEWORKERDDI RtlUnicodeStringInitWorker
(
_Out_ PUNICODE_STRING DestinationString,
_In_opt_ NTSTRSAFE_PCWSTR pszSrc,
_In_ const size_t cchMax,
_In_ DWORD dwFlags
)
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

NTSTRSAFEDDI RtlUnicodeStringInit
(
_Out_ PUNICODE_STRING DestinationString,
_In_opt_ NTSTRSAFE_PCWSTR pszSrc
)
{
	return RtlUnicodeStringInitWorker(DestinationString, pszSrc, NTSTRSAFE_UNICODE_STRING_MAX_CCH, 0);
}
