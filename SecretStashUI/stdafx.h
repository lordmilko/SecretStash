#pragma once

#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#define UNICODE
#define _UNICODE

//#define GUID_DEFS_ONLY //Prevent atldlgs.h from eventually including shldisp.h

#include <atlbase.h>
#include <atlapp.h>
#include <atlframe.h>
#include <atlctrls.h>
#include <atlmisc.h>
#include <atlribbon.h>