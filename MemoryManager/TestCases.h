#ifndef TEST_CASES
#define TEST_CASES

#include "Defines.h"
#include "MemoryManager.hpp"
#include "GlutManager.hpp"

/*
	char 1 byte
	short 2 bytes
	float 4 bytes
	double 8 bytes
*/

class TestCases {
private:
	MemoryManager& memMngr;
	GlutManager& glutMngr;

public:
	TestCases();
	~TestCases();

	void someTest();
	void anotherTest();

	void cleanMemoryManager();

	void testPointerSafetySingle();

	
	

	void runSingleThreaded();
	void runMultiThreaded();
	void efficiencyTest();
	void compareEfficiencySingleThreaded(int capacityExponent, int entrySizeExponent);

	void testCase2();
	void testCase4();
	void testCase8();
	void testCase9();
	void testCase10();
	void testCase11();

};

#endif //TEST_CASES
