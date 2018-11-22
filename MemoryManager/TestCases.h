#ifndef TEST_CASES
#define TEST_CASES

#include "Defines.h"
#include "MemoryManager.hpp"

class TestCases {
private:
	MemoryManager& memMngr;

public:
	TestCases();
	~TestCases();

	void run();

};

#endif //TEST_CASES
