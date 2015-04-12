#pragma once

#include <fltKernel.h>
#include <dontuse.h>
#include <suppress.h>

#define LeaveOnFail(action) status = (action); if(!NT_SUCCESS(status)) leave

LONG ShouldHandleException(PEXCEPTION_POINTERS ExceptionPointer, BOOLEAN AccessingUserBuffer);