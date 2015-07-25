#include "pe_bliss.h"

#ifdef PE_BLISS_WINDOWS
#ifndef _M_X64
#ifdef _DEBUG
#pragma comment(lib, "../Debug/pe_bliss.lib")
#else
#pragma comment(lib, "../Release/pe_bliss.lib")
#endif
#else
#ifdef _DEBUG
#pragma comment(lib, "../x64/Debug/pe_bliss.lib")
#else
#pragma comment(lib, "../x64/Release/pe_bliss.lib")
#endif
#endif
#endif

int load_executable(const char* executable);
	