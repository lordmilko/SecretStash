#pragma once

#if _DEBUG
#define DBG_PRINT(...) \
    DbgPrint(__VA_ARGS__)
#else
#define DBG_PRINT(...)  UNREFERENCED_PARAMETER(__VA_ARGS__)
#endif