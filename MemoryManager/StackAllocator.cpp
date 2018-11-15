#include "StackAllocator.hpp"

Marker StackAllocator::getMarker()
{
	return m_marker;
}

void StackAllocator::clearToMarker(Marker marker)
{
}

void * StackAllocator::allocate(size_t sizeBytes)
{
	return nullptr;
}

void StackAllocator::deallocateAll()
{
}
