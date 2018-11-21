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

public:
	StackAllocator(void* memPtr, size_t sizeBytes);
	virtual ~StackAllocator();

	virtual void* allocate(size_t sizeBytes);
	virtual void deallocateAll();

	Marker getMarker();
	void clearToMarker(Marker marker);

	void cleanUp();
};

#endif //STACK_ALLOCATOR