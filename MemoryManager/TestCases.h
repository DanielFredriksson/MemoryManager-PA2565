#ifndef TEST_CASES
#define TEST_CASES

#include "Defines.h"
#include "ThreadManager.h"
#include "MemoryManager.hpp"

class TestCases {
private:
	MemoryManager& memMngr;
	ThreadManager& threadMngr;

public:
	TestCases();
	~TestCases();

	void runSingleThreaded();
	void runMultiThreaded();

};

#endif //TEST_CASES
