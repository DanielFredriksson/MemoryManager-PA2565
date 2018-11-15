#pragma once

#include "Allocator.h"

typedef unsigned int Marker;
class StackAllocator : private Allocator 
{
private:
	Marker m_marker;

public:
	StackAllocator(void* memPtr, size_t sizeBytes);
	virtual ~StackAllocator();

	virtual void* allocate(size_t sizeBytes);
	virtual void deallocateAll();

	Marker getMarker();
	void clearToMarker(Marker marker);
};