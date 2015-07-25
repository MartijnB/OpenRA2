#include "stdafx.h"

using namespace pe_bliss;

#pragma bss_seg(".engine")
unsigned char __loader_space[32 * 1024 * 1024];
#pragma bss_seg()

void __print_error_msg()
{
	const TCHAR* lpMsgBuf;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	std::wcout << lpMsgBuf << std::endl;
}

int load_sections(pe_base image)
{
	std::cout << "Reading PE sections..." << std::hex << std::showbase << std::endl;
	const section_list sections(image.get_image_sections());

	for (section_list::const_iterator it = sections.begin(); it != sections.end(); ++it) {
		const section& s = *it;

		uint32_t section_va = image.rva_to_va_32(s.get_virtual_address());
		uint32_t section_size = s.get_virtual_size();

		std::cout << "Load section " << s.get_name() << " @ " << section_va << " (" << section_size << ")";

		memcpy((void*)section_va, (void*)s.get_raw_data().c_str(), min(section_size, s.get_raw_data().length()));

		if (s.executable() && s.writeable()) {
			std::cout << " [RWX]" << std::endl;

			VirtualProtect((void*)section_va, section_size, PAGE_EXECUTE_READWRITE, NULL);
		}
		else if (s.executable()) {
			std::cout << " [R X]" << std::endl;

			VirtualProtect((void*)section_va, section_size, PAGE_EXECUTE_READ, NULL);
		}
		else if (s.writeable()) {
			std::cout << " [RW ]" << std::endl;

			VirtualProtect((void*)section_va, section_size, PAGE_READWRITE, NULL);
		}
		else {
			std::cout << " [R  ]" << std::endl;

			VirtualProtect((void*)section_va, section_size, PAGE_READONLY, NULL);
		}
	}

	return 0;
}

int load_imports(pe_base image)
{
	if (image.has_imports()) {
		const imported_functions_list imports = get_imported_functions(image);

		for (imported_functions_list::const_iterator it = imports.begin(); it != imports.end(); ++it) {
			const import_library& lib = *it;
			
			std::cout << "Load library " << lib.get_name() << "... ";

			HMODULE lib_handle = LoadLibrary(lib.get_name().c_str());

			if (lib_handle == NULL) {
				__print_error_msg();

				return -1;
			}

			PULONG lib_iat_va = (PULONG)image.rva_to_va_32(lib.get_rva_to_iat());
			unsigned int lib_func_nr = 0;

			const import_library::imported_list& functions = lib.get_imported_functions();
			for (import_library::imported_list::const_iterator func_it = functions.begin(); func_it != functions.end(); ++func_it) {
				const imported_function& func = *func_it;

				FARPROC func_handle = NULL;
				if (func.has_name()) {
					func_handle = GetProcAddress(lib_handle, func.get_name().c_str());
				}
				else {
					func_handle = GetProcAddress(lib_handle, MAKEINTRESOURCEA(func.get_ordinal()));
				}

				if (func_handle == NULL) {
					std::cout << "Can't find ";

					if (func.has_name())
						std::cout << func.get_name();
					else
						std::cout << "#" << std::dec << func.get_ordinal();

					std::cout << "!";
				}

				lib_iat_va[lib_func_nr] = (ULONG)func_handle;

				lib_func_nr++;
			}

			std::cout << std::dec << lib_func_nr <<  " symbol(s) imported!" << std::endl;
		}
	}

	return 0;
}

int load_executable(const char* executable)
{
	std::cout << "Loader segment: " << std::hex << (void*)__loader_space << " - " << (void*)(__loader_space + sizeof(__loader_space)) << std::endl;

	std::ifstream pe_file(executable, std::ios::in | std::ios::binary);
	if (!pe_file) {
		std::cout << "Cannot open " << executable << std::endl;
		return -1;
	}

	try
	{
		pe_base image(pe_factory::create_pe(pe_file));

		if (load_sections(image)) {
			return -1;
		}

		if (load_imports(image)) {
			return -1;
		}
	}
	catch (const pe_exception& e)
	{
		std::cout << "Error: " << e.what() << std::endl;
		return -1;
	}

	return 0;
}