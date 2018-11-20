#include <iostream>
#include <memory>
#include "MemoryManager.hpp"
#include <thread>
#include <chrono>
#include <ctime>

void testStackAllocator() {
	std::mutex funcOutput;

	void* ptr = calloc(1, 500);
	StackAllocator stack(ptr, 500);
	std::chrono::system_clock::time_point tt = std::chrono::system_clock::now() + std::chrono::seconds(3);
	auto func = [&stack, &tt, &funcOutput]() {
		std::this_thread::sleep_until(tt);
		void* tmpPtr = stack.allocate(200);


		std::lock_guard<std::mutex> lock(funcOutput);
		std::cout << tmpPtr << std::endl;
	};



	std::thread t1(func);
	std::thread t2(func);
	std::thread t3(func);

	std::thread t4(std::bind(&StackAllocator::allocate, &stack, 100));

	t1.join();
	t2.join();
	t3.join();
	t4.join();

	std::cout << std::endl << stack.getMarker();
}

int main() 
{	
	// Testing multiple threads for stack allocator
	testStackAllocator();

	system("pause");

	return EXIT_SUCCESS;
}