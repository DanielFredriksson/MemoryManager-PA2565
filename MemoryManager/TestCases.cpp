#include "TestCases.h"

#include <chrono>

TestCases::TestCases()
	: memMngr(MemoryManager::getInstance())
	, threadMngr(ThreadManager::getInstance())
{
}

TestCases::~TestCases()
{
}

void TestCases::runSingleThreaded()
{
	/*
	*	Pool Allocator
	*/
	// Single-threaded Allocation - Pointer Safety
	std::cout << "" << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(1));
	std::cout << "----" << std::endl;

	// Single-threaded Allocation - Overflow handling
	std::cout << "" << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(1));
	std::cout << "----" << std::endl;

	// Single-threaded Deallocation - Reallocation
	std::cout << "" << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(1));
	std::cout << "----" << std::endl;

	// Single-threaded Allocation - Efficiency compared to native allocation
	std::cout << "" << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(1));
	std::cout << "----" << std::endl;
}

void TestCases::runMultiThreaded()
{
	threadMngr.init(4U);
	memMngr.setThreads(threadMngr.getThreadIDs());
	memMngr.addPool(ARCH_BYTESIZE, 4);


	// Multi-threaded Allocation - Pointer Safety
	std::cout << "Multi-threaded Allocation - Pointer Safety" << std::endl;
	
	std::mutex mutex;
	auto func1 = [&mutex]() {
		auto& _memMngr = MemoryManager::getInstance();
		void* ptr = _memMngr.randomAllocate(ARCH_BYTESIZE);
		
		std::lock_guard<std::mutex> lock(mutex);
		std::cout << ptr << std::endl;
	};
	
	for (int i = 0; i < 4; i++)
		threadMngr.appendJob(func1);
	
	std::this_thread::sleep_for(std::chrono::seconds(1));
	system("pause");
	std::cout << "----" << std::endl;
	// -----

	memMngr.deallocateAllRandom();
	std::this_thread::sleep_for(std::chrono::seconds(1));


	// Multi-threaded Allocation - Thread Safety
	std::cout << "Multi-threaded Allocation - Thread Safety" << std::endl;

	std::chrono::system_clock::time_point sleepTill = std::chrono::system_clock::now() + std::chrono::seconds(2);
	auto func2 = [&mutex, &sleepTill]() {
		auto& _memMngr = MemoryManager::getInstance();
		std::this_thread::sleep_until(sleepTill);
		void* ptr = _memMngr.randomAllocate(ARCH_BYTESIZE);

		std::lock_guard<std::mutex> lock(mutex);
		std::cout << ptr << std::endl;
	};

	for (int i = 0; i < 4; i++)
		threadMngr.appendJob(func2);

	std::this_thread::sleep_for(std::chrono::seconds(3));
	system("pause");
	std::cout << "----" << std::endl;
	// -----


	// Multi-threaded Allocation - Overflow handling
	std::cout << "Multi-threaded Allocation - Overflow handling" << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(1));
	std::cout << "----" << std::endl;
	// -----

	// Multi-threaded Allocation - No allocation during deallocation
	std::cout << "Multi-threaded Allocation - No allocation during deallocation" << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(1));
	std::cout << "----" << std::endl;
	// -----

	// Multi-threaded Allocation - Efficiency compared to single-threaded allocation
	std::cout << "Multi-threaded Allocation - Efficiency compared to single-threaded allocation" << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(1));
	std::cout << "----" << std::endl;
	// -----

	// Multi-threaded Allocation - Efficiency compared to native allocation
	std::cout << "" << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(1));
	std::cout << "----" << std::endl;
	// -----

}
