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

int main()
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
			auto ptr8 =  malloc(size);
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
	std::thread t2(func);


	system("pause");
	return EXIT_SUCCESS;
}