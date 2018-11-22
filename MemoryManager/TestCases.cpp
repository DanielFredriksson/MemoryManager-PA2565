#include "TestCases.h"

TestCases::TestCases() 
	: memMngr(MemoryManager::getInstance())
{
}

TestCases::~TestCases()
{
}

void TestCases::run()
{
	auto& memMngr = MemoryManager::getInstance();
	// Single-threaded Allocation - Pointer Safety

	// Single-threaded Allocation - Overflow handling

	// Single-threaded Deallocation - Reallocation

	// Single-threaded Allocation - Efficiency compared to native allocation

	// Multi-threaded Allocation - Pointer Safety

	// Multi-threaded Allocation - Thread Safety

	// Multi-threaded Allocation - Overflow handling

	// Multi-threaded Allocation - No allocation during deallocation

	// Multi-threaded Allocation - Efficiency compared to single-threaded allocation

	// Multi-threaded Allocation - Efficiency compared to native allocation
}
