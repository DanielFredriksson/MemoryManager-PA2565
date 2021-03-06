#include "Defines.h"
#include "MemoryManager.h"
#include "GlutManager.h"
#include "TestCases.h"

int main()
{
	// Keep main clean! Make tests in TestCases.h/TestCases.cpp
	TestCases testCases;
	testCases.timingTest(8, 8000, 100);
	testCases.poolAllocDealloc();

	return EXIT_SUCCESS;
}