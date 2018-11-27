#include "Defines.h"
#include "MemoryManager.h"
#include "GlutManager.h"
#include "TestCases.h"

int main()
{
	// Keep main clean! Make tests in TestCases.hpp/TestCases.cpp
	TestCases testCases;
	//testCases.testCase4(8, 8000, 100);
	//testCases.testCase4(16, 8000, 100);
	//testCases.testCase4(32, 8000, 100);
	//testCases.testCase4(64, 8000, 100);
	//testCases.testCase4(128, 8000, 100);
	//testCases.testCase4(256, 8000, 100);
	//testCases.testCase4(512, 8000, 100);
	//testCases.testCase4(1024, 8000, 100);

	//testCases.testCase13();
	testCases.poolAllocDealloc();

	return EXIT_SUCCESS;
}