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
	this->cleanUp();
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
	if (this->m_memPtr != nullptr)
	{
		free(this->m_memPtr);
		this->m_memPtr = nullptr;
	}
}