#include "StackAllocator.hpp"


Marker StackAllocator::getMarker()
{
	return m_marker;
}

void StackAllocator::clearToMarker(Marker marker)
{
	m_marker.store(marker);
}

StackAllocator::StackAllocator(void * memPtr, size_t sizeBytes) : Allocator(memPtr, sizeBytes)
{
	m_marker.store(0);
}

StackAllocator::~StackAllocator()
{
	cleanUp();
}

void * StackAllocator::allocate(size_t sizeBytes)
{
	char* ptr = nullptr;

	std::lock_guard<std::shared_mutex> lock(m_mtx);
	try {
		if (m_marker + sizeBytes <= m_sizeBytes) {
			// Get current marker location and move marker to top.
			Marker currMarker = m_marker.fetch_add(sizeBytes);

			ptr = (char*)this->m_memPtr + currMarker;
		}
		else
			throw 1;
	}
	catch (int e) {
		std::cout << "StackAllocator::allocate : An EXCEPTION occured! Exception #" << e << '\n';
	}
	return ptr;
}

void StackAllocator::deallocateAll()
{
	m_marker.store(0);
}

void StackAllocator::cleanUp()
{
	if (m_memPtr != nullptr)
	{
		free(m_memPtr);
		m_memPtr = nullptr;
	}
}