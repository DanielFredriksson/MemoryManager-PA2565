#include "TestCases.h"

#include <chrono>
#include <string>
#include <stdexcept>

#include <future> // used to 'get' results from threads

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

void TestCases::testCase3()
{
	std::cout << "\n\n\n\n";
	std::cout << "TEST: Single-threaded Deallocation - Reallocation" << "\n";
	std::cout << "DESCRIPTION: Using a single thread, it should be possible to allocate to memory that has previously been deallocated." << "\n\n";

	// Clean up the memory manager from any previous test
	this->memMngr.cleanUp();
	std::cout << "Initializing test: The MEMORY MANAGER has now been cleaned in case of any lingering data." << "\n";
	std::cout << "Press 'ENTER' to start. NOTE: For each step, CONTINUE by pressing 'ENTER'";
	std::getchar();
	std::cout << "\n\n";

	/// STACK (INIT) ----------------
	unsigned int stackEntryCount = 5;
	unsigned int stackSize = 8 * stackEntryCount;
	unsigned int stackEntrySize = 8;
	// ------------------------------

	/// POOL (INIT) ----------------------
	unsigned int poolEntrySize = 8;
	unsigned int poolEntryCount = 8;
	std::vector<void*> poolEntryAddresses;
	unsigned int poolQuadrantCount = 1;
	// -----------------------------------

	// Setting up our 'poolsList' and our single pool b4 adding it to the memMngr
	std::vector<MemoryManager::PoolInstance> poolsList;
	MemoryManager::PoolInstance testPool = {
		poolEntrySize,
		poolEntryCount,
		poolQuadrantCount
	};
	poolsList.push_back(testPool);
	// Initiate a single stack and our single pool
	memMngr.init(stackSize, poolsList);

	// Resizing our list of POOL ADDRESSES
	poolEntryAddresses.resize(poolEntryCount);

	std::cout << "1. A stack (entrySize: " << stackEntrySize << " bytes, totalSize: " << stackEntrySize << " * " << stackEntryCount << ") and a pool (entrySize: " << poolEntrySize << " bytes, total size: " << poolEntryCount << ") have been initialized." << "\n";
	std::getchar();
	std::cout << "\n\n";

	/// STACK (TEST) -------------------------------------------------------------
	std::cout << "2. Testing stack ALLOCATION followed by COMPLETE DEALLOCATION:";
	std::getchar();
	std::cout << "\n";

	memMngr.singleFrameAllocate(stackEntrySize);
	memMngr.deallocateStack();
	std::cout << "\t" << "Action: Single-entry allocation followed by deallocation: SUCCESS";
	std::getchar();
	std::cout << "\n";

	for (int i = 0; i < stackEntryCount; i++)
		memMngr.singleFrameAllocate(stackEntrySize);

	// ERROR CHECKING; POSSIBLE THROW
	if (memMngr.getStack()->getUsedBytesCount() != (stackEntrySize * stackEntryCount))
		throw "ERROR: 5 allocations EXACTLY on the stack expected, but NOT FOUND!";

	memMngr.deallocateStack();
	
	// ERROR CHECKING; POSSIBLE THROW
	if (memMngr.getStack()->getUsedBytesCount() != 0)
		throw "ERROR: 0 allocations on the stack expected, but SOMETHING FOUND!";

	std::cout << "\t" << "Action: Multiple-entry (count = 5) allocation followed by deallocation: SUCCESS";
	std::getchar();
	std::cout << "\n\n";

	std::cout << "3. Testing pool ALLOCATION and DEALLOCATION:";
	std::getchar();
	std::cout << "\n";
	// ----------------------------------------------------------------------------------------------------

	/// POOL (TEST) ------------------------------------------------------------------------
	// Allocate a single pool entry at a time, saving the addresses
	for (int i = 0; i < poolEntryCount; i++)
		poolEntryAddresses.at(i) = memMngr.randomAllocate(8);
	
	// ERROR CHECKING; POSSIBLE THROW
	if (memMngr.getSpecificPool(0)->getFreeEntriesCount() != 0)
		throw "ERROR: Expected the pool to be FULLY ALLOCATED, but it WASN'T!";

	std::cout << "\t" << "Action: Repeatedly allocate until pool is full: SUCCESS";
	std::getchar();
	std::cout << "\n";

	for (int i = 0; i < poolEntryCount; i++)
	{	// Deallocating a single pool entry at a time, using saved addresses
		memMngr.deallocateSingleRandom(poolEntryAddresses.at(i), poolEntrySize);
		// Reallocating each address RIGHT after deallocation
		poolEntryAddresses.at(i) = memMngr.randomAllocate(poolEntrySize);
	}

	if (memMngr.getSpecificPool(0)->getFreeEntriesCount() != 0)
		throw "ERROR: Expected the pool to be FULLY ALLOCATED AGAIN, but it WASN'T!";

	std::cout << "\t" << "Action: Deallocate and reallocate each entry, one by one: SUCCESS";
	std::getchar();
	std::cout << "\n";

	// Deallocating all entries from our pool
	memMngr.deallocateAllRandom();

	if (memMngr.getSpecificPool(0)->getFreeEntriesCount() != poolEntryCount)
		throw "ERROR: Expected the pool to be FULLY DEALLOCATED, but it WASN'T!";

	std::cout << "\t" << "Action: Deallocate ALL pool entries: SUCCESS";
	std::cout << "\n";
	// --------------------------------------------------------------------------------------
}

void TestCases::testCase8()
{
	std::cout << "TEST: Multi-threaded Deallocation - Reallocation" << std::endl;
	std::cout << "DESCRIPTION: Using multi-threading, it should be possible to reallocate memory that has been deallocated previously." << std::endl << std::endl;
	
	/// Fetch memorymanager and clean it from earlier shit.
	this->memMngr.cleanUp();
	/// Determine Stacks and pools with the same capacity
	// Stack
	unsigned int byteCapacity = 8;
	// Pool
	unsigned int poolEntryByteSize = 8;
	unsigned int numEntries = byteCapacity / poolEntryByteSize;
	unsigned int numQuadrants = 1;
	std::vector<MemoryManager::PoolInstance> pools;
	MemoryManager::PoolInstance pool = {
		poolEntryByteSize,
		numEntries,
		numQuadrants
	};
	pools.push_back(pool);
	// Initiate memorymanager with stack and pool
	memMngr.init(byteCapacity, pools);

	/// Allocate
	std::cout << "Allocating memory..." << std::endl;
	MemoryManager& instance = MemoryManager::getInstance();
	void* adressPointer = instance.randomAllocate(8);
	std::cout << "Memory has been allocated." << std::endl << std::endl;

	/// Functions
	// Allocate
	std::chrono::system_clock::time_point sleepTill;
	auto allocateFunction = [&sleepTill]() {
		try {
			MemoryManager& instance = MemoryManager::getInstance();
			return instance.randomAllocate(8);
		} 
		catch (std::exception &e) {
			std::cout << "Error caught! [ " << e.what() << std::endl << std::endl;
		}
	};
	// Deallocate
	auto deallocateFunction = [&sleepTill, &adressPointer]() {
		try {
			MemoryManager& instance = MemoryManager::getInstance();
			instance.deallocateSingleRandom(adressPointer, 8);
		}
		catch (std::exception &e) {
			std::cout << "Error caught! [ " << e.what() << std::endl << std::endl;
		}
	};

	sleepTill = std::chrono::system_clock::now() + std::chrono::seconds(2);
	std::cout << "Attempting to deallocate and allocate simultaneously in: 2 seconds." << std::endl;

	std::thread t1(deallocateFunction);
	std::thread t2(allocateFunction);
	std::this_thread::sleep_until(sleepTill + std::chrono::seconds(1));
	std::cout << "If no error was caught, it was a success!" << std::endl << std::endl;

	t1.join();
	t2.join();
}

void TestCases::testCase9()
{
	std::cout << "TEST:  Multi-threaded Allocation - No allocation during deallocation." << std::endl;
	std::cout << "DESCRIPTION:  It shouldn’t be possible to allocate new memory while all the memory is being deallocated." << std::endl << std::endl;
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

	std::cout << "TEST: Multi-threaded Allocation - Overflow handling" << std::endl;
	std::cout << "DESCRIPTION: Trying to allocate more memory than is available should throw an exception" << std::endl;

	/// START
	// Allocate more memory than is available in the stack
	auto func1 = []() {
		try {
			std::cout << "Attempting to overflow the stack..." << std::endl;
			while (true) {
				MemoryManager::getInstance().singleFrameAllocate(8);
			}
		}
		catch (std::exception &e) {
			std::cout << "Error caught! [ " << e.what() << " ]" << std::endl;
		}
	};
	
	auto func2 = []() {
		std::cout << "Will be stuck here since attempting to allocate in the pool will just keep looking after memory forever" << std::endl;
		try {
			std::cout << "Attempting to overflow the pool..." << std::endl;
			while (true) {
				MemoryManager::getInstance().randomAllocate(8);
			}
		}
		catch (std::exception &e) {
				std::cout << "Error caught! [ " << e.what() << " ]" << std::endl;
		}
	};

	std::thread t1(func1);
	t1.join();
	std::thread t2(func2);
	
	t1.join();
	t2.join();
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

void TestCases::testCase4() 
{
	auto testFunc = []() {
		MemoryManager& memMgr = MemoryManager::getInstance();
		std::vector<MemoryManager::PoolInstance> pi;

		memMgr.cleanUp();

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

	t1.join();
}

void TestCases::testCase10()
{
	// Setup the memory manager
	cleanMemoryManager();
	



	// Output
	/*std::cout << "Capacity: " << byteCapacity << ". EntrySize: " << poolEntryByteSize << std::endl;
	std::cout << "Memorymanager took: " << differenceOurs.count() << " nanoseconds" << std::endl;
	std::cout << "New took: " << differenceNew.count() << " nanoseconds" << std::endl;
	std::cout << "Ours was better by: " << difference.count() << " nanoseconds" << std::endl;
	std::cout << std::endl << std::endl;*/
}

void TestCases::testCase11()
{
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
