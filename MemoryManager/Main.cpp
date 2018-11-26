#include <iostream>
#include <thread>
#include <deque>

// TEMP
#include <chrono>         // std::chrono::system_clock
#include <ctime>          // std::time_t, std::tm, std::localtime, std::mktime

#include "Defines.h"
#include "MemoryManager.hpp"

#include "GlutManager.hpp"
#include "TestCases.h"

int main()
{
	// Keep main clean! Make tests in TestCases.hpp/TestCases.cpp
	TestCases testCases;
	//testCases.poolAllocDealloc();
	testCases.testCase4(64, 8000);
	testCases.testCase4(128, 8000);
	testCases.testCase4(256, 8000);
	/*for (int i = 0; i < 10; i++)
		testCases.testCase4();*/

	system("pause");
	return EXIT_SUCCESS;
}