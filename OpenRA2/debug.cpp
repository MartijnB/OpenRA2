#include "stdafx.h"

#include "engine.h"

void debug_print(const char * format, ...)
{
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
}

REPLACE_ENGINE_FUNCTION(nullsub_1, debug_print)

// Disable launcher check
char return1() { return 1; }

REPLACE_ENGINE_FUNCTION(sub_49F620, return1)

_BYTE* dword_884E5C = (_BYTE*)0x884E5C;
_BYTE* dword_884E58 = (_BYTE*)0x884E58;

char sub_534DE0_()
{
	if (*dword_884E5C)
	{
		nullsub_1("     Releasing %s\n", *(_DWORD *)(dword_884E5C + 12));
		if (*dword_884E5C)
			(**(void(__stdcall ***)(_DWORD))*dword_884E5C)(1);
		dword_884E5C = 0;
	}
	if (*dword_884E58)
	{
		nullsub_1("     Releasing %s\n", *(_DWORD *)(dword_884E58 + 12));
		if (*dword_884E58)
			(**(void(__stdcall ***)(_DWORD))*dword_884E58)(1);
		dword_884E58 = 0;
	}
	return 1;
}

REPLACE_ENGINE_FUNCTION(sub_534DE0, sub_534DE0_)


//void __cdecl lock(int a1);

//REPLACE_ENGINE_FUNCTION(__lock, lock)

/*
LPCRITICAL_SECTION* dword_87C2A8 = (LPCRITICAL_SECTION*)0x87C2A8;

void __cdecl lock(int a1)
{
	LPCRITICAL_SECTION v1; // edi@2

	if (!*(dword_87C2A8 + a1))
	{
		v1 = (LPCRITICAL_SECTION)malloc(0x18u);
		if (!v1)
			_amsg_exit(0x11u);
		_lock(17);
		if (*(dword_87C2A8 + a1))
		{
			free(v1);
		}
		else
		{
			InitializeCriticalSection(v1);
			*(dword_87C2A8 + a1) = v1;
		}
		_unlock(17);
	}
	EnterCriticalSection(*(dword_87C2A8 + a1));
}
*/