#include "StackAllocator.hpp"

Marker StackAllocator::getMarker()
{
	return m_marker;
}

void StackAllocator::clearToMarker(Marker marker)
{
}

StackAllocator::StackAllocator(void * memPtr, size_t sizeBytes)
{
}

StackAllocator::~StackAllocator()
{
}

void * StackAllocator::allocate(size_t sizeBytes)
{
	return nullptr;
}

void StackAllocator::deallocateAll()
{
}
