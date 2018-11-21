#ifndef STACK_ALLOCATOR
#define STACK_ALLOCATOR

#include "Allocator.h"
#include <atomic>
#include <shared_mutex>
#include <iostream>

typedef unsigned int Marker;
class StackAllocator : private Allocator 
{
private:
	std::atomic<Marker> m_marker;
	std::shared_mutex m_mtx;


private:
	void cleanUp();

	unsigned int padMemory(unsigned int sizeBytes);


public:
	StackAllocator(void* memPtr, unsigned int sizeBytes);
	virtual ~StackAllocator();

	void* allocate(unsigned int sizeBytes);
	virtual void deallocateAll();

	Marker getMarker();
	void clearToMarker(Marker marker);


};

#endif //STACK_ALLOCATOR