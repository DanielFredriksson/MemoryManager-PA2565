#include <iostream>
#include <thread>
#include <deque>

// TEMP
#include <chrono>         // std::chrono::system_clock
#include <ctime>          // std::time_t, std::tm, std::localtime, std::mktime

#include "Defines.h"
#include "MemoryManager.hpp"

#include "GlutManager.hpp"
#include "TestCases.h"

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

int main()
{
	/*
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

	GlutManager& instance = GlutManager::getInstance();
	MemoryManager& memMgr = MemoryManager::getInstance();
	std::vector<MemoryManager::PoolInstance> pi;

	pi.push_back(MemoryManager::PoolInstance{ 8, 10, 4 });
	pi.push_back(MemoryManager::PoolInstance{ 8 * 2, 10, 4 });
	pi.push_back(MemoryManager::PoolInstance{ 8 * 3, 10, 4 });

	memMgr.init(1024, pi);

	auto ptr = memMgr.randomAllocate(5);

	//system("pause");
	return EXIT_SUCCESS;
}