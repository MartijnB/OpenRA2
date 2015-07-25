#undef ENGINE_FUNCTION

typedef DWORD _DWORD;
typedef DWORD _UNKNOWN;

typedef unsigned char _BYTE;

typedef unsigned short u_short;

#if defined GENERATE_ENGINE_SYMBOLS
#define ENGINE_FUNCTION(name, calling_convention, return_type, signature, address) return_type(calling_convention *name)signature = (return_type(calling_convention *)signature)address;
#elif defined GENERATE_ENGINE_SYMBOL_LIST
#define ENGINE_FUNCTION(name, calling_convention, return_type, signature, address) { #name, #name #signature, name },
#else
#define ENGINE_FUNCTION(name, calling_convention, return_type, signature, address) extern return_type (calling_convention *name)signature;
#define REPLACE_ENGINE_FUNCTION(old_name, new_name) int __REF_ ## old_name ## _ ## new_name = register_replace_engine_function(old_name, #new_name, new_name);
#endif

#if defined GENERATE_ENGINE_SYMBOL_LIST
struct engine_symbol
{
	char* name;
	char* signature;
	void* address;
};

struct engine_symbol g_engine_symbols[] =
{
#endif

ENGINE_FUNCTION(start_engine, __cdecl, void, (), 0x007CD80F)

#include "external_symbols_list.h"

#if defined GENERATE_ENGINE_SYMBOL_LIST
};
#endif

const char* lookup_external_function_name(void* symbol_address);
const char* lookup_external_function_signature(void* symbol_address);

int register_replace_engine_function(void* old_function, const char* new_function_name, void* new_function_address);

void patch_engine_symbols();