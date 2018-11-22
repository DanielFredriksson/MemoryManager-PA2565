#ifndef TEST_CASES
#define TEST_CASES

#include "Defines.h"
#include "ThreadManager.h"
#include "MemoryManager.hpp"
#include "GlutManager.hpp"

class TestCases {
private:
	MemoryManager& memMngr;
	ThreadManager& threadMngr;
	GlutManager& glutMngr;

public:
	TestCases();
	~TestCases();

	void runSingleThreaded();
	void runMultiThreaded();

};

#endif //TEST_CASES
