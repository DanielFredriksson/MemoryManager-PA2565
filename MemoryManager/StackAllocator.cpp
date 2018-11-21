#include "StackAllocator.hpp"


Marker StackAllocator::getMarker()
{
	return m_marker;
}

void StackAllocator::clearToMarker(Marker marker)
{
}

StackAllocator::StackAllocator(void * memPtr, size_t sizeBytes) : Allocator(memPtr, sizeBytes)
{
}

StackAllocator::~StackAllocator()
{
	cleanUp();
}

void * StackAllocator::allocate(size_t sizeBytes)
{
	m_marker.fetch_add(sizeBytes);

	return nullptr;
}

void StackAllocator::deallocateAll()
{
}

void StackAllocator::cleanUp()
{
	if (m_memPtr != nullptr)
	{
		free(m_memPtr);
		m_memPtr = nullptr;
	}
}