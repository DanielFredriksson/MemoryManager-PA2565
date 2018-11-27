#ifndef STACK_ALLOCATOR
#define STACK_ALLOCATOR

#include "Allocator.h"
#include <atomic>
#include <shared_mutex>
#include <iostream>

// DEFINITIONS
// -------------------------
typedef unsigned int Marker;
// -------------------------

class StackAllocator : private Allocator 
{
private: /// VARIABLES
	std::atomic<Marker> m_marker;
	std::shared_mutex m_mtx;

private: /// FUNCTIONS
	unsigned int padMemory(unsigned int sizeBytes);
	void cleanUp();

public: /// FUNCTIONS
	StackAllocator(void* memPtr, unsigned int sizeBytes);
	virtual ~StackAllocator();

	void* allocate(unsigned int sizeBytes);
	virtual void deallocateAll();

	Marker getMarker();
	void clearToMarker(Marker marker);

	// Memory tracking for GLUT (Drawing our stack)
	virtual std::vector<bool> getUsedMemory();
};

#endif //STACK_ALLOCATOR
