#include "TestCases.h"

#include <chrono>
#include <string>

TestCases::TestCases()
	: memMngr(MemoryManager::getInstance())
	, glutMngr(GlutManager::getInstance())
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
	std::vector<MemoryManager::PoolInstance> poolInstances;
	poolInstances.push_back(MemoryManager::PoolInstance{ ARCH_BYTESIZE, 12 });
	memMngr.init(10U * ARCH_BYTESIZE, poolInstances);
	//memMngr.addPool(ARCH_BYTESIZE, 4);


	// Multi-threaded Allocation - Pointer Safety
	std::cout << "Multi-threaded Allocation - Pointer Safety" << std::endl;
	
	std::mutex mutex;
	auto func1 = [&mutex]() {
		auto& _memMngr = MemoryManager::getInstance();
		void* ptr = _memMngr.randomAllocate(ARCH_BYTESIZE);
		
		std::lock_guard<std::mutex> lock(mutex);
		std::cout << ptr << std::endl;
	};
	
	//for (int i = 0; i < 12; i++)
		//threadMngr.appendJob(func1);
	
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

	//for (int i = 0; i < 12; i++)
		//threadMngr.appendJob(func2);

	std::this_thread::sleep_for(std::chrono::seconds(3));
	system("pause");
	std::cout << "----" << std::endl;
	// -----

	std::this_thread::sleep_for(std::chrono::seconds(1));

	// Multi-threaded Allocation - Overflow handling
	std::cout << "Multi-threaded Allocation - Overflow handling" << std::endl;
	auto func3 = [&mutex]() {
		auto& _memMngr = MemoryManager::getInstance();
		std::string error = "";
		try {
			void* ptr = _memMngr.randomAllocate(ARCH_BYTESIZE);
		}
		catch (std::exception& e) {
			error = e.what();
		}

		std::lock_guard<std::mutex> lock(mutex);
		std::cout << "Error: " << error << std::endl;
	};

	//for (int i = 0; i < 4; i++)
		//threadMngr.appendJob(func3);
	
	std::this_thread::sleep_for(std::chrono::seconds(1));
	getchar();
	std::cout << "----" << std::endl;
	// -----

	std::this_thread::sleep_for(std::chrono::seconds(1));

	// Multi-threaded Allocation - No allocation during deallocation
	std::cout << "Multi-threaded Allocation - No allocation during deallocation" << std::endl;
	// Hm...
	std::this_thread::sleep_for(std::chrono::seconds(1));
	std::cout << "----" << std::endl;
	// -----

}

void TestCases::efficiencyTest()
{
	//threadMngr.init(4U);
	auto numAllocations = 2000U;
	std::atomic_int aInt = ATOMIC_VAR_INIT(0);
	auto func = [&aInt]() {
		//auto& _memMngr = MemoryManager::getInstance();
		//void* ptr = _memMngr.randomAllocate(ARCH_BYTESIZE);
		std::cout << "t";
		//std::cout << aInt << std::endl;
	};
	//for (int i = 0; i < numAllocations; i++)
		//threadMngr.appendJob(func);

	while (true);

	//while (true) std::cout << aInt << std::endl;

	/*memMngr.setThreads(threadMngr.getThreadIDs());
	std::vector<MemoryManager::PoolInstance> poolInstances;
	poolInstances.push_back(MemoryManager::PoolInstance{ ARCH_BYTESIZE, numAllocations });
	memMngr.init(10U * ARCH_BYTESIZE, poolInstances);

	// Multi-threaded Allocation - Efficiency compared to single-threaded allocation
	std::cout << "Multi-threaded Allocation - Efficiency compared to single-threaded allocation" << std::endl;
	
	std::mutex mutex;	
	std::this_thread::sleep_for(std::chrono::seconds(1));
	std::cout << "----" << std::endl;
	// -----

	// Multi-threaded Allocation - Efficiency compared to native allocation
	std::cout << "Multi-threaded Allocation - Efficiency compared to native allocation" << std::endl;
	
	//std::vector<std::thread> threads(numAllocations);
	auto t1 = std::chrono::high_resolution_clock::now() + std::chrono::seconds(5);
	
	std::atomic_int aInt = ATOMIC_VAR_INIT(0);
	auto func = [&aInt]() {
		//auto& _memMngr = MemoryManager::getInstance();
		//void* ptr = _memMngr.randomAllocate(ARCH_BYTESIZE);
		aInt.fetch_add(1);
		//std::cout << aInt << std::endl;
	};
	
	/*std::vector<std::thread::id> threadIDs;
	for (unsigned int i = 0; i < numAllocations; i++) {
		threads.at(i) = std::thread(func);
		threadIDs.push_back(threads.at(i).get_id());
	}
	memMngr.setThreads(threadIDs);
	// TEMP
	std::vector<MemoryManager::PoolInstance> poolInstances;
	poolInstances.push_back(MemoryManager::PoolInstance{ ARCH_BYTESIZE, numAllocations });
	memMngr.init(10U * ARCH_BYTESIZE, poolInstances);
	for (int i = 0; i < numAllocations; i++)
		threads[i].join();*/
	// TEMP

	/*for (int i = 0; i < numAllocations; i++)
		threadMngr.appendJob(func);
	//std::this_thread::sleep_for(std::chrono::seconds(8));
	//memMngr.updateAllocatedSpace();
	while (aInt != numAllocations)
		std::cout << aInt << std::endl;
	auto t2 = std::chrono::high_resolution_clock::now();
	std::cout << "MemoryManager::randomAllocate() took "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()
		<< " milliseconds\n";

	auto t3 = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < numAllocations; i++)
		void* ptr = malloc(ARCH_BYTESIZE);
	auto t4 = std::chrono::high_resolution_clock::now();
	std::cout << "malloc() took "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(t4 - t3).count()
		<< " milliseconds\n";

	std::this_thread::sleep_for(std::chrono::seconds(1));
	std::cout << "----" << std::endl;
	// -----*/
}
