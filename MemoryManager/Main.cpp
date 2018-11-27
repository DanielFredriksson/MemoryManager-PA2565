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

	/*MemoryManager& memMgr = MemoryManager::getInstance();
	std::vector<PoolInstance> pi;
	unsigned int size = 64; // The size of a 4x4 matrix of floats
	unsigned int numAssignments = ARCH_BYTESIZE * 50;
	unsigned int maxSizeBytes = size * numAssignments;
	pi.push_back(PoolInstance{ size, numAssignments, 4 });
	memMgr.init(ARCH_BYTESIZE, pi);

	std::cout << memMgr.randomAllocate(size) << std::endl;*/
	
	
	system("pause");
	return EXIT_SUCCESS;
}