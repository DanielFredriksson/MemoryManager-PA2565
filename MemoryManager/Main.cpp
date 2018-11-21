#include <iostream>
#include <memory>
#include "MemoryManager.hpp"
#include <thread>
#include <chrono>
#include <ctime>

void testMultithreadStack() {
	
	std::mutex funcOutput;
	
	std::chrono::system_clock::time_point tt = std::chrono::system_clock::now() + std::chrono::seconds(3);
	
	// Creating a stack with 500 bytes
	void* ptr = calloc(1, 500);
	StackAllocator stack(ptr, 500);

	auto func = [&stack, &tt, &funcOutput]() {
		std::this_thread::sleep_until(tt);
		void* tmpPtr = stack.allocate(200);
		std::lock_guard<std::mutex> lock(funcOutput);
		std::cout << tmpPtr << std::endl;
	};

	// Trying to allocate a total of 600 bytes from 3 different threads at the same time
	std::thread t1(func);
	std::thread t2(func);
	std::thread t3(func);

	std::thread t4(std::bind(&StackAllocator::allocate, &stack, 100));

	t1.join();
	t2.join();
	t3.join();
	t4.join();

	std::cout << std::endl << stack.getMarker();

	free(ptr);
}

int main() 
{	
	// Testing multiple threads for stack allocator
	testMultithreadStack();

	system("pause");

	return EXIT_SUCCESS;
}