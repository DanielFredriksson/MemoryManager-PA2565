#ifndef STACK_ALLOCATOR
#define STACK_ALLOCATOR

#include <atomic>

#include "Allocator.h"

typedef unsigned int Marker;
class StackAllocator : private Allocator 
{
private:
	std::atomic<Marker> m_marker;

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