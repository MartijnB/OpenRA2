#include "stdafx.h"

#include "engine.h"

#define GENERATE_ENGINE_SYMBOLS
#include "external_symbols.h"
#undef GENERATE_ENGINE_SYMBOLS

#define GENERATE_ENGINE_SYMBOL_LIST
#include "external_symbols.h"

struct replace_engine_function {
	void* old_address;
	void* new_address;

	const char* new_name;
};

struct replace_engine_function* g_replace_engine_function_list[1024] = { 0 };
unsigned int g_replace_engine_function_list_used = 0;

int register_replace_engine_function(void* old_function, const char* new_function_name, void* new_function_address)
{
	struct replace_engine_function* r = new replace_engine_function();
	r->old_address = old_function;
	r->new_address = new_function_address;

	r->new_name = new_function_name;

	g_replace_engine_function_list[g_replace_engine_function_list_used++] = r;

	return 1;
}

const char* lookup_external_function_name(void* symbol_address) {
	for (int i = 0; i < sizeof(g_engine_symbols) / sizeof(struct engine_symbol); i++) {
		if (g_engine_symbols[i].address == symbol_address) {
			return g_engine_symbols[i].name;
		}
	}

	return 0;
}

const char* lookup_external_function_signature(void* symbol_address) {
	for (int i = 0; i < sizeof(g_engine_symbols); i++) {
		if (g_engine_symbols[i].address == symbol_address) {
			return g_engine_symbols[i].signature;
		}
	}

	return 0;
}

#define WRITE_BYTE(base, offset, value) *((unsigned char*)(((unsigned long)base) + offset)) = (unsigned char) value;
#define WRITE_LONG(base, offset, value) *((unsigned long*)(((unsigned long)base) + offset)) = (unsigned long) value;

void patch_engine_symbols()
{
	for (unsigned int i = 0; i < g_replace_engine_function_list_used; i++) {
		const char* old_name = lookup_external_function_name(g_replace_engine_function_list[i]->old_address);
		const char* new_name = lookup_external_function_name(g_replace_engine_function_list[i]->new_address);

		if (!old_name)
			old_name = "<UNKNOWN>";

		if (!new_name)
			if (g_replace_engine_function_list[i]->new_name)
				new_name = g_replace_engine_function_list[i]->new_name;
			else
				new_name = "<UNKNOWN>";

		std::cout << "Replace " << old_name << " with " << new_name << std::endl;

		// 0:  2E FF 25 EF BE AD DE    jmp    DWORD PTR cs : 0xDEADBEEF
		WRITE_BYTE(g_replace_engine_function_list[i]->old_address, 0, 0x2E);
		WRITE_BYTE(g_replace_engine_function_list[i]->old_address, 1, 0xFF);
		WRITE_BYTE(g_replace_engine_function_list[i]->old_address, 2, 0x25);
		WRITE_LONG(g_replace_engine_function_list[i]->old_address, 3, &g_replace_engine_function_list[i]->new_address);
	}
}