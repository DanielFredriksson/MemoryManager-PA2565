#ifndef TEST_CASES
#define TEST_CASES

#include "Defines.h"
#include "MemoryManager.hpp"
#include "GlutManager.hpp"

class TestCases {
private:
	MemoryManager& memMngr;
	GlutManager& glutMngr;

public:
	TestCases();
	~TestCases();

	void runSingleThreaded();
	void runMultiThreaded();
	void efficiencyTest();

};

#endif //TEST_CASES
