#include "TestCases.h"

#include <chrono>
#include <string>
#include <time.h>
#include <stdlib.h>
#include "Defines.h"
#include <future>
#include <random>

#include <future> // used to 'get' results from threads

TestCases::TestCases()
	: memMngr(MemoryManager::getInstance())
	, glutMngr(GlutManager::getInstance())
{
}

TestCases::~TestCases()
{
}

void TestCases::cleanMemoryManager() {
	memMngr.cleanUp();
	
	std::vector<PoolInstance> pi;
	pi.push_back(PoolInstance{ 512, 8000, 4 });
	pi.push_back(PoolInstance{ 256, 8000, 4 });
	pi.push_back(PoolInstance{ 128, 8000, 4 });
	memMngr.init(1024, pi);
}

void TestCases::testCase4(unsigned int sizePerAlloc, unsigned int numAllocs, unsigned int numIter) 
{
	double averages[3];
	averages[0] = 0;
	averages[1] = 0;
	averages[2] = 0;
 	auto testFunc = [&sizePerAlloc, &numAllocs, &averages]() {
		MemoryManager& memMgr = MemoryManager::getInstance();
		std::vector<PoolInstance> pi;

		memMgr.cleanUp();

		unsigned int size = sizePerAlloc;
		unsigned int numAllocations = numAllocs;
		pi.push_back(PoolInstance{ size, numAllocations, 4 });

		memMgr.init(size * numAllocations, pi);

		// Pool allocation
		auto start = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < numAllocations; i++) {
			void* ptr = memMgr.randomAllocate(size);
		}
		auto end = std::chrono::high_resolution_clock::now();
		auto timeSpan = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
		averages[0] += timeSpan.count();

		// Stack allocation
		start = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < numAllocations; i++) {
			void* ptr = memMgr.singleFrameAllocate(size);
		}
		end = std::chrono::high_resolution_clock::now();
		timeSpan = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
		averages[1] += timeSpan.count();

		// Malloc
		start = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < numAllocations; i++)
			void* ptr = malloc(size);
		end = std::chrono::high_resolution_clock::now();
		timeSpan = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
		averages[2] += timeSpan.count();
	};

	for (int i = 0; i < numIter; i++) {
		std::thread t1(testFunc);

		t1.join();
	}

	for(unsigned int i = 0; i < 3; i++)
		averages[i] /= static_cast<double>(numIter);

	std::cout << "Size of each allocation [" << sizePerAlloc << " bytes]. Number of allocations [" << numAllocs << "]. Number of iterations: "  << numIter << "." << std::endl;
	std::cout << "Our pool allocation took on average: \t" << averages[0] << " seconds." << std::endl;
	std::cout << "Our stack allocation took on average: \t" << averages[1] << " seconds." << std::endl;
	std::cout << "Native malloc took on average: \t\t" << averages[2] << " seconds." << std::endl;

}

void TestCases::poolAllocDealloc()
{
	MemoryManager& memMgr = MemoryManager::getInstance();
	memMgr.cleanUp();

	std::vector<PoolInstance> pi;

	unsigned int size = 64; // The size of a 4x4 matrix of floats
	unsigned int numAssignments = ARCH_BYTESIZE * 50;
	unsigned int maxSizeBytes = size * numAssignments;
	pi.push_back(PoolInstance{ size, numAssignments, 4 });

	memMgr.init(size * numAssignments, pi);

	auto testFunc = [&numAssignments, &size]() {
		MemoryManager& memMgr = MemoryManager::getInstance();

		srand(time(0));
		std::vector<void*> poolPointers;
		while (true) {
			try {
				if (rand() % 10 < 4 || poolPointers.size() < numAssignments / 8) {
					poolPointers.push_back(memMgr.randomAllocate(size));
				}
				else {
					if (poolPointers.size() > 0) {
						unsigned int index = rand() % poolPointers.size();
						memMgr.deallocateSingleRandom(poolPointers[index], size);
						std::swap(poolPointers[index], poolPointers.back());
						poolPointers.pop_back();
					}
				}

				if (rand() % 100 < 90) {
					memMgr.singleFrameAllocate(rand() % 256);
				}
				else {
					memMgr.deallocateStack();
				}

			}
			catch (std::exception& e) {
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(40));
		}

	};
	std::thread t1(testFunc);
	std::thread t2(testFunc);
	std::thread t3(testFunc);
	std::thread t4(testFunc);

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
	std::thread t5(allocatedSpaceUpdateFunction);
	GlutManager& glutMngr = GlutManager::getInstance();
	glutMngr.EnterMainLoop();

	t1.join();
	t2.join();
	t3.join();
	t4.join();
	t5.join();
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
