#include "TestCases.h"

#include <chrono>
#include <string>
#include <future>
#include <random>

TestCases::TestCases()
	: memMngr(MemoryManager::getInstance())
	, glutMngr(GlutManager::getInstance())
{
}

TestCases::~TestCases()
{
}

void TestCases::anotherTest()
{
	auto testFunc = []() {
		MemoryManager& memMgr = MemoryManager::getInstance();
		std::vector<MemoryManager::PoolInstance> pi;

		unsigned int size = 2048;
		unsigned int numAssignments = 8000;
		std::cout << "Number of allocation (" << numAssignments << ")  Size of each allocation (" << size << ")" << std::endl;
		pi.push_back(MemoryManager::PoolInstance{ size, numAssignments, 4 });
		//pi.push_back(MemoryManager::PoolInstance{ 8 * 2, 12, 4 });
		//pi.push_back(MemoryManager::PoolInstance{ 8 * 3, 12, 4 });

		memMgr.init(1024, pi);

		auto start = std::chrono::system_clock::now();
		for (int i = 0; i < numAssignments; i++) {
			void* ptr = memMgr.randomAllocate(size);
		}
		auto end = std::chrono::system_clock::now();
		std::cout << "Ours took: \n" << (end - start).count() << std::endl;


		start = std::chrono::system_clock::now();
		for (int i = 0; i < numAssignments; i++)
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

void TestCases::cleanMemoryManager() {
	memMngr.cleanUp();
	
	std::vector<MemoryManager::PoolInstance> pi;
	pi.push_back(MemoryManager::PoolInstance{ 512, 8000, 4 });
	pi.push_back(MemoryManager::PoolInstance{ 256, 8000, 4 });
	pi.push_back(MemoryManager::PoolInstance{ 128, 8000, 4 });
	memMngr.init(1024, pi);
}

void TestCases::testCase2()
{
	/// Fetch memorymanager and clean it from earlier shit.
	this->memMngr.cleanUp();
	/// Determine Stacks and pools with the same capacity
	// Stack
	unsigned int byteCapacity = 32;
	// Pool
	unsigned int poolEntryByteSize = 8;
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

	std::cout << "TEST: Single-threaded Allocation - Overflow handling" << std::endl;
	std::cout << "DESCRIPTION: Trying to allocate more memory than is available should throw an exception" << std::endl << std::endl;

	/// START
	// Allocate more memory than is available in the stack
	try {
		std::cout << "Attempting to overflow the stack..." << std::endl;
		while (true) {
			this->memMngr.singleFrameAllocate(8);
		}
	}
	catch (std::exception &e) {
		std::cout << "Error caught! [ " << e.what() << " ]" << std::endl;
	}
	// Allocate more memory than is available in the stack
	try {
		std::cout << "Attempting to overflow the pool..." << std::endl;
		while (true) {
			this->memMngr.randomAllocate(8);
		}
	}
	catch (std::exception &e) {
		std::cout << "Error caught! [ " << e.what() << " ]" << std::endl << std::endl;
	}
}

void TestCases::testPointerSafetySingle() {
	// test ID# 1
	std::cout << "ID# 1 : Single-threaded Allocation - Pointer Safety";
	std::cout << "\n\nPool Allocation:\n";
	cleanMemoryManager();
	bool failed = false;
	void *ptr1 = nullptr;
	void *ptr2 = nullptr;

	for (unsigned int i = 0; i < 2000; i++) {
		ptr1 = memMngr.randomAllocate(100);
		if (ptr2 == ptr1) {
			std::cout << "Pool Allocation failed! Allocating to same memory!\n";
			failed = true;
		}
		else
			ptr2 = ptr1;
	}

	if (!failed) {
		std::cout << "Successful.\n";
	}

	std::cout << "\n\nStack Allocation:\n";

	ptr1 = nullptr;
	ptr2 = ptr1;
	for (unsigned int i = 0; i < 100; i++) {
		ptr1 = memMngr.singleFrameAllocate(8);
		if (ptr2 == ptr1) {
			std::cout << "Stack Allocation failed! Allocating to same memory!\n";
			failed = true;
		}
		else
			ptr2 = ptr1;
	}
	if (!failed) {
		std::cout << "Successful.\n";
	}

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

void TestCases::testCase4(unsigned int sizePerAlloc, unsigned int numAllocs) 
{
	auto testFunc = [&sizePerAlloc, &numAllocs]() {
		MemoryManager& memMgr = MemoryManager::getInstance();
		std::vector<MemoryManager::PoolInstance> pi;

		memMgr.cleanUp();

		unsigned int size = sizePerAlloc;
		unsigned int numAllocations = numAllocs;
		pi.push_back(MemoryManager::PoolInstance{ size, numAllocations, 4 });

		memMgr.init(size * numAllocations, pi);

		std::cout << "Size of each allocation [" << size << " bytes]. Number of allocations [" << numAllocations << "]." << std::endl;

		auto start = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < numAllocations; i++) {
			void* ptr = memMgr.randomAllocate(size);
		}
		auto end = std::chrono::high_resolution_clock::now();
		auto timeSpan = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
		std::cout << "Our pool allocation took: \t" << timeSpan.count() << " seconds." << std::endl;

		start = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < numAllocations; i++) {
			void* ptr = memMgr.singleFrameAllocate(size);
		}
		end = std::chrono::high_resolution_clock::now();
		timeSpan = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
		std::cout << "Our stack allocation took: \t" << timeSpan.count() << " seconds." << std::endl;


		start = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < numAllocations; i++)
			void* ptr = malloc(size);
		end = std::chrono::high_resolution_clock::now();
		timeSpan = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
		std::cout << "Native malloc took: \t\t" << timeSpan.count() << " seconds." << std::endl;
	};
	std::thread t1(testFunc);

	t1.join();
}

void TestCases::testCase10()
{
	unsigned int size = 64;
	unsigned int numAllocations = 8000;
	unsigned int numThreads = 4U;
	auto testFunc = [&size, &numAllocations, &numThreads]() {
		MemoryManager& memMgr = MemoryManager::getInstance();
		std::vector<MemoryManager::PoolInstance> pi;

		memMgr.cleanUp();

		pi.push_back(MemoryManager::PoolInstance{ size, numAllocations, numThreads });

		memMgr.init(size * numAllocations, pi);

		std::cout << "Size of each allocation [" << size << " bytes]. Number of allocations [" << numAllocations << "]." << std::endl;

		auto start = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < numAllocations; i++) {
			void* ptr = memMgr.randomAllocate(size);
		}
		auto end = std::chrono::high_resolution_clock::now();
		auto time_span = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
		std::cout << "[Single-threaded] Our pool allocation took: \t" << time_span.count() << " nanoseconds." << std::endl;

		start = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < numAllocations; i++) {
			void* ptr = memMgr.singleFrameAllocate(size);
		}
		end = std::chrono::high_resolution_clock::now();
		time_span = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
		std::cout << "[Single-threaded] Our stack allocation took: \t" << time_span.count() << " seconds." << std::endl;
		
	};

	std::thread t1(testFunc);
	t1.join();

	MemoryManager& memMgr = MemoryManager::getInstance();
	std::vector<MemoryManager::PoolInstance> pi;

	memMgr.cleanUp();

	pi.push_back(MemoryManager::PoolInstance{ size, numAllocations, numThreads });

	memMgr.init(size * numAllocations, pi);


	
	auto poolAlloc = [&size, &numAllocations, &numThreads]() {

		auto& memMgr = MemoryManager::getInstance();
		unsigned int allocations = numAllocations / numThreads;
		
		auto start = std::chrono::high_resolution_clock::now();

		for (int i = 0; i < allocations; i++)
			void* ptr = memMgr.randomAllocate(size);

		auto end = std::chrono::high_resolution_clock::now();
		return std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
	};

	auto stackStart = std::chrono::high_resolution_clock::now() + std::chrono::seconds(4);
	auto stackAlloc = [&size, &numAllocations, &stackStart, &numThreads]() {

		auto& memMgr = MemoryManager::getInstance();
		unsigned int allocations = numAllocations / numThreads;
		auto start = std::chrono::high_resolution_clock::now();

		for (int i = 0; i < allocations; i++)
			void* ptr = memMgr.singleFrameAllocate(size);

		auto end = std::chrono::high_resolution_clock::now();
		return std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
	};

	std::vector<std::future<std::chrono::duration<double>>> threads(numThreads);

	for (int i = 0; i < numThreads; i++) {
		threads.at(i) = std::async(std::launch::async, poolAlloc);
	}
	std::chrono::duration<double> timeElapsed = std::chrono::duration<double>(0.0);
	for (int i = 0; i < numThreads; i++) {
		try {
			timeElapsed += threads.at(i).get();
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl;
		}
	}
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << "[Multi-threaded] Our pool allocation took: \t" << timeElapsed.count() << " seconds." << std::endl;


	for (int i = 0; i < numThreads; i++) {
		threads.at(i) = std::async(std::launch::async, stackAlloc);
	}
	timeElapsed = std::chrono::duration<double>(0.0);
	for (int i = 0; i < numThreads; i++) {
		try {
			timeElapsed += threads.at(i).get();
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl;
		}
	}
	std::cout << "[Multi-threaded] Our stack allocation took: \t" << timeElapsed.count() << " seconds." << std::endl;

}

void TestCases::testCase11()
{
}

void TestCases::poolAllocDealloc()
{
	auto testFunc = []() {
		MemoryManager& memMgr = MemoryManager::getInstance();
		std::vector<MemoryManager::PoolInstance> pi;

		unsigned int size = 64; // The size of a 4x4 matrix of floats
		unsigned int numAssignments = ARCH_BYTESIZE * 10;
		unsigned int maxSizeBytes = size * numAssignments;
		pi.push_back(MemoryManager::PoolInstance{ size, numAssignments, 4 });

		memMgr.init(ARCH_BYTESIZE, pi);

		srand(time(0));
		std::vector<void*> pointers;
		while (true) {
			try {
				if (rand() % 10 < 4 || pointers.size() < numAssignments / 2) {
					pointers.push_back(memMgr.randomAllocate(size));
					std::cout << "Added element" << std::endl;
				}
				else {
					if (pointers.size() > 0) {
						unsigned int index = rand() % pointers.size();
						memMgr.deallocateSingleRandom(pointers[index], size);
						std::swap(pointers[index], pointers.back());
						pointers.pop_back();
						std::cout << "Added element" << std::endl;
					}
				}

			}
			catch (std::exception& e) {
				
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(40));
		}

	};
	std::thread t1(testFunc);

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
