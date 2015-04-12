#include "Exception.h"

LONG ShouldHandleException(PEXCEPTION_POINTERS ExceptionPointer, BOOLEAN AccessingUserBuffer)
{
	NTSTATUS status = ExceptionPointer->ExceptionRecord->ExceptionCode;

	//We only want to handle exceptions for accessing user buffers
	if (!FsRtlIsNtstatusExpected(status) && !AccessingUserBuffer)
		return EXCEPTION_CONTINUE_SEARCH;

	return EXCEPTION_EXECUTE_HANDLER;
}