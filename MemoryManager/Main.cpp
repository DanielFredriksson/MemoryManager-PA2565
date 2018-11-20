#include <iostream>
#include <thread>
#include <deque>

// TEMP
#include <chrono>         // std::chrono::system_clock
#include <ctime>          // std::time_t, std::tm, std::localtime, std::mktime

#include "ThreadManager.h"
#include "MemoryManager.hpp"

void testThreadSafetyOnPoolAllocator() {
	auto& memMngr = MemoryManager::getInstance();
	auto pAlloc = memMngr.addPool(50, 10);

	std::chrono::system_clock::time_point sleepTill = std::chrono::system_clock::now() + std::chrono::seconds(10);
	auto tFunc = [&pAlloc, &sleepTill]() {
		std::this_thread::sleep_until(sleepTill);
		pAlloc->allocate();
	};

	pAlloc->printEntries();

	std::deque<std::thread> threads;
	for (int i = 0; i < 10; i++)
		threads.emplace_back(std::thread(tFunc));

	std::cout << "-------" << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(10));
	pAlloc->printEntries();

	for (int i = 0; i < 10; i++)
		threads[i].join();
}

void testThreadManager() 
{
	ThreadManager& tMngr = ThreadManager::getInstance();
	tMngr.init(50U);

	auto tFunc = []() {
		std::cout << "T";
	};

	for (size_t i = 0; i < 10; i++)
		tMngr.appendJob(tFunc);

	std::this_thread::sleep_for(std::chrono::seconds(2));
	std::cout << std::endl;

	for (size_t i = 0; i < 10; i++)
		tMngr.appendJob(tFunc);

	std::this_thread::sleep_for(std::chrono::seconds(1));
	std::cout << std::endl;
}

int main()
{

	system("pause");
	return EXIT_SUCCESS;
}