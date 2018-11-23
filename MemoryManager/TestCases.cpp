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

void TestCases::someTest()
{
	GlutManager& instance = GlutManager::getInstance();
	MemoryManager& memMgr = MemoryManager::getInstance();
	std::vector<MemoryManager::PoolInstance> pi;

	pi.push_back(MemoryManager::PoolInstance{ 8, 12, 4 });
	pi.push_back(MemoryManager::PoolInstance{ 8 * 2, 12, 4 });
	pi.push_back(MemoryManager::PoolInstance{ 8 * 3, 12, 4 });

	memMgr.init(1024, pi);

	auto ptr = memMgr.randomAllocate(5);
	auto ptr2 = memMgr.randomAllocate(5);
	auto ptr3 = memMgr.randomAllocate(5);
	auto ptr4 = memMgr.randomAllocate(5);
	void* ptr1;
	auto func = [&ptr1, &memMgr]() {
		ptr1 = memMgr.randomAllocate(5);
	};

}

void TestCases::anotherTest()
{
	auto testFunc = []() {
		MemoryManager& memMgr = MemoryManager::getInstance();
		std::vector<MemoryManager::PoolInstance> pi;


		unsigned int size = 512;
		pi.push_back(MemoryManager::PoolInstance{ size, 8000, 4 });
		//pi.push_back(MemoryManager::PoolInstance{ 8 * 2, 12, 4 });
		//pi.push_back(MemoryManager::PoolInstance{ 8 * 3, 12, 4 });

		memMgr.init(1024, pi);

		auto start = std::chrono::system_clock::now();
		for (int i = 0; i < 8000; i++) {
			void* ptr = memMgr.randomAllocate(size);
		}
		auto end = std::chrono::system_clock::now();
		std::cout << "Ours took: \n" << (end - start).count() << std::endl;


		start = std::chrono::system_clock::now();
		for (int i = 0; i < 8000; i++)
			auto ptr8 = malloc(size);
		end = std::chrono::system_clock::now();
		std::cout << "Malloc took: \n" << (end - start).count() << std::endl;
	};
	std::thread t1(testFunc);

	auto testFuncMulti = []() {
		MemoryManager& memMgr = MemoryManager::getInstance();
		std::vector<MemoryManager::PoolInstance> pi;

		pi.push_back(MemoryManager::PoolInstance{ 24, 8 * 800, 4 });
		pi.push_back(MemoryManager::PoolInstance{ 16, 8 * 16, 4 });
		memMgr.init(1024, pi);

		std::chrono::system_clock::time_point sleepTill = std::chrono::system_clock::now() + std::chrono::seconds(2);

		auto func = [&memMgr, &sleepTill]() {
			std::this_thread::sleep_until(sleepTill);
			auto ptr = memMgr.randomAllocate(5);
			for (int i = 0; i < 2; i++)
				auto ptr = memMgr.randomAllocate(9);
		};

		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		std::vector<std::thread> tVec(8);
		for (int j = 0; j < 8; j++) {
			for (int i = 0; i < 8; i++) {
				tVec.at(i) = std::thread(func);
			}

			for (int i = 0; i < tVec.size(); i++)
				tVec.at(i).join();

			std::this_thread::sleep_for(std::chrono::milliseconds(200));
		}
	};
	//std::thread t1(testFuncMulti);

	auto allocatedSpaceUpdateFunction = []() {
		GlutManager& glutMngr = GlutManager::getInstance();
		MemoryManager& memMngr = MemoryManager::getInstance();

		auto allocatedSpace = memMngr.getAllocatedSpace();
		std::vector<std::vector<bool>> stacks(1);
		std::this_thread::sleep_for(std::chrono::milliseconds(500));

		// Update the vectors of the GUI
		while (true) {
			auto allocatedSpace = memMngr.getAllocatedSpace();
			stacks.at(0) = allocatedSpace.stacks;
			glutMngr.updateVectors(stacks, allocatedSpace.pools);
			memMngr.updateAllocatedSpace();
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
		}
	};
	std::this_thread::sleep_for(std::chrono::seconds(1));
	std::thread t2(allocatedSpaceUpdateFunction);
	GlutManager& glutMngr = GlutManager::getInstance();
	glutMngr.EnterMainLoop();

	t1.join();
	t2.join();
	//TestCases testcases;
	//testcases.efficiencyTest();

	/*std::thread t1(func);
	std::thread t2(func);*/
}

void TestCases::testCase2()
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

void TestCases::compareEfficiencySingleThreaded(int capacityExponent, int entrySizeExponent)
{
	using namespace std::chrono;
	// Fetch memorymanager and clean it from earlier shit.
	this->memMngr.cleanUp();


	/// Determine Stacks and pools with the same capacity
	// Stack
	unsigned int byteCapacity = pow(2, capacityExponent);
	// Pool
	unsigned int poolEntryByteSize = pow(2, entrySizeExponent);
	unsigned int numEntries = byteCapacity / poolEntryByteSize;
	unsigned int numQuadrants = 4;
	std::vector<MemoryManager::PoolInstance> pools;
	MemoryManager::PoolInstance pool = {
		poolEntryByteSize,
		numEntries,
		numQuadrants
	};
	pools.push_back(pool);
	// Initiate memorymanager with stack and pool
	memMngr.init(byteCapacity, pools);


	/// Allocate a bunch of shit with memoryManager & time it
	// Start the clock!
	auto startOurs = high_resolution_clock::now();
	// Fill the stack and pool with doubles
	for (int i = 0; i < numEntries; i++) {
		double * adress = (double*)memMngr.singleFrameAllocate(poolEntryByteSize);
		adress = (double*)memMngr.randomAllocate(poolEntryByteSize);
	}
	auto endOurs = high_resolution_clock::now();
	auto differenceOurs = duration_cast<std::chrono::nanoseconds>(endOurs - startOurs);

	/// Allocate a bunch of shit with new & time it
	auto startNew = high_resolution_clock::now();
	for (int i = 0; i < numEntries; i++) {
		double* adress = (double*)malloc(poolEntryByteSize);
		adress = (double*)malloc(poolEntryByteSize);
	}
	auto endNew = high_resolution_clock::now();
	auto differenceNew = duration_cast<std::chrono::nanoseconds>(endNew - startNew);

	/// What was the difference?
	auto difference = duration_cast<std::chrono::nanoseconds>(differenceNew - differenceOurs);
	auto our = duration_cast<nanoseconds>(differenceOurs);
	auto news = duration_cast<nanoseconds>(differenceNew);
	auto scale = our / news;

	// Output
	std::cout << "Capacity: " << byteCapacity << ". EntrySize: " << poolEntryByteSize << std::endl;
	std::cout << "Memorymanager took: " << differenceOurs.count() << " nanoseconds" << std::endl;
	std::cout << "New took: " << differenceNew.count() << " nanoseconds" << std::endl;
	std::cout << "Ours was better by: " << difference.count() << " nanoseconds" << std::endl;
	std::cout << "Ours was worse by the ratio: " << scale << std::endl;
	std::cout << std::endl << std::endl;
}


// ------------------- OUTCOMMENTED OLD SHIT -------------------


//void testThreadSafetyOnPoolAllocator() 
//{
//	auto& memMngr = MemoryManager::getInstance();
//	auto pAlloc = memMngr.addPool(50, 10);
//
//	std::chrono::system_clock::time_point sleepTill = std::chrono::system_clock::now() + std::chrono::seconds(10);
//	auto tFunc = [&pAlloc, &sleepTill]() {
//		std::this_thread::sleep_until(sleepTill);
//		pAlloc->allocate();
//	};
//
//	pAlloc->printEntries();
//
//	std::deque<std::thread> threads;
//	for (int i = 0; i < 10; i++)
//		threads.emplace_back(std::thread(tFunc));
//
//	std::cout << "-------" << std::endl;
//	std::this_thread::sleep_for(std::chrono::seconds(10));
//	pAlloc->printEntries();
//
//	for (int i = 0; i < 10; i++)
//		threads[i].join();
//}

//void testThreadManager() 
//{
//	ThreadManager& tMngr = ThreadManager::getInstance();
//	tMngr.init(50U);
//
//	auto tFunc = []() {
//		std::cout << "T";
//	};
//
//	for (size_t i = 0; i < 10; i++)
//		tMngr.appendJob(tFunc);
//
//	std::this_thread::sleep_for(std::chrono::seconds(2));
//	std::cout << std::endl;
//
//	for (size_t i = 0; i < 10; i++)
//		tMngr.appendJob(tFunc);
//
//	std::this_thread::sleep_for(std::chrono::seconds(1));
//	std::cout << std::endl;
//}
//
//void testMultithreadStack() 
//{
//
//	std::mutex funcOutput;
//
//	std::chrono::system_clock::time_point tt = std::chrono::system_clock::now() + std::chrono::seconds(3);
//
//	// Creating a stack with 700 bytes
//	auto& MemMgr = MemoryManager::getInstance();
//	auto stack = MemMgr.addStack(700);
//	
//
//	auto func = [&stack, &tt, &funcOutput]() {
//		std::this_thread::sleep_until(tt);
//		void* tmpPtr;
//		try {
//			tmpPtr = stack->allocate(200);
//		}
//		catch (std::exception& e) {
//			std::cout << e.what() << std::endl;
//			exit(EXIT_FAILURE);
//		}
//		std::lock_guard<std::mutex> lock(funcOutput);
//		std::cout << tmpPtr << std::endl;
//	};
//
//	// Trying to allocate a total of 600 bytes from 3 different threads at the same time
//	std::thread t1(func);
//	std::thread t2(func);
//	std::thread t3(func);
//
//	std::thread t4(std::bind(&StackAllocator::allocate, stack, 100));
//
//	t1.join();
//	t2.join();
//	t3.join();
//	t4.join();
//
//	std::cout << std::endl << stack->getMarker();
//}


	/*
{
	auto testFunc = []() {
		std::cout << "t";
	};

	ThreadManager& tMngr = ThreadManager::getInstance();
	tMngr.init(4U);
	std::this_thread::sleep_for(std::chrono::seconds(1));
	for (int i = 0; i < 1000; i++) {
		tMngr.appendJob(testFunc);
	}*/

	/*TestCases testCases;
	auto testCasesFunc = [&testCases]() {
		std::this_thread::sleep_for(std::chrono::seconds(2));
		testCases.runMultiThreaded();
	};
	std::thread t1(testCasesFunc);

	auto allocatedSpaceUpdateFunction = []() {
		GlutManager& glutMngr = GlutManager::getInstance();
		MemoryManager& memMngr = MemoryManager::getInstance();

		auto allocatedSpace = memMngr.getAllocatedSpace();
		std::vector<std::vector<bool>> stacks(1);
		std::this_thread::sleep_for(std::chrono::seconds(2));

		// Update the vectors of the GUI
		while (true) {
			auto allocatedSpace = memMngr.getAllocatedSpace();
			stacks.at(0) = allocatedSpace.stacks;
			glutMngr.updateVectors(stacks, allocatedSpace.pools);
			memMngr.updateAllocatedSpace();
		}
	};
	std::this_thread::sleep_for(std::chrono::seconds(1));
	std::thread t2(allocatedSpaceUpdateFunction);

	ptr->EnterMainLoop();

	t1.join();
	t2.join();*/
	//TestCases testcases;
	//testcases.efficiencyTest();
//}
