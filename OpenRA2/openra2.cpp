#include "stdafx.h"

#include "engine.h"

int main()
{
	if (load_executable(RA2_EXE)) {
		return 0;
	}

	std::cout << "Patching engine symbols..." << std::endl;

	patch_engine_symbols();

	//Fool the engine that the launcher is running
	CreateMutexA(0, 0, "48BC11BD-C4D7-466b-8A31-C6ABBAD47B3E");

	std::cout << "Start engine..." << std::endl;

	start_engine();
	
	return 0;
}

