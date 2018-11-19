#include <iostream>
#include <memory>
#include <thread>
#include <deque>

// TEMP
#include <chrono>         // std::chrono::system_clock
#include <ctime>          // std::time_t, std::tm, std::localtime, std::mktime

#include "MemoryManager.hpp"
#include "PoolAllocator.hpp"

void testThreadSafetyOnPoolAllocator() {
	void* ptr = malloc(500);
	auto pAlloc = PoolAllocator(ptr, 50, 10);

	std::chrono::system_clock::time_point sleepTill = std::chrono::system_clock::now() + std::chrono::seconds(10);
	auto tFunc = [&pAlloc, &sleepTill]() {
		std::this_thread::sleep_until(sleepTill);
		pAlloc.allocate();
	};

	pAlloc.printEntires();

	std::deque<std::thread> threads;
	for (int i = 0; i < 10; i++)
		threads.emplace_back(std::thread(tFunc));

	std::cout << "-------" << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(10));
	pAlloc.printEntires();

	for (int i = 0; i < 10; i++)
		threads[i].join();
}

int main() 
{	

	return EXIT_SUCCESS;
}