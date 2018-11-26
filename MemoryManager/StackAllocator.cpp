#include "Defines.h"
#include "StackAllocator.hpp"


Marker StackAllocator::getMarker()
{
	return m_marker;
}

void StackAllocator::clearToMarker(Marker marker)
{
	m_marker.store(marker);
}

std::vector<bool> StackAllocator::getUsedMemory()
{
	std::vector<bool> usedMemory;
	unsigned int percentUsed = int(float(m_marker) / float(m_sizeBytes) * 100.f);
	for (unsigned int i = 0; i < 100; i++)
		usedMemory.push_back((i < percentUsed) ? true : false);
	return usedMemory;
}

int StackAllocator::getUsedBytesCount()
{
	int returnValue = 0;
	char* stackBottomPointer = static_cast<char*>(m_memPtr);
	char* stackTopPointer = (static_cast<char*>(m_memPtr) + m_marker);

	while (stackBottomPointer != stackTopPointer)
	{
		returnValue++;
		stackBottomPointer++;
	}

	return returnValue;
}

StackAllocator::StackAllocator(void * memPtr, unsigned int sizeBytes): Allocator(memPtr, padMemory(sizeBytes))
{
	m_marker.store(0);
}

StackAllocator::~StackAllocator()
{
	cleanUp();
}

void* StackAllocator::allocate(unsigned int sizeBytes)
{
	char* ptr = nullptr;
	unsigned int paddedMemory = padMemory(sizeBytes);
	

	std::lock_guard<std::shared_mutex> lock(m_mtx);
	//try {
		if (m_marker + paddedMemory <= m_sizeBytes) {
			// Get current marker location and move marker to top.
			Marker currMarker = m_marker.fetch_add(paddedMemory);

			ptr = (char*)m_memPtr + currMarker;
		}
		else
			throw std::overflow_error("StackAllocator::allocate : Tried to allocate more memory than was available.");
	//}
	//catch (int e) {
	//	std::cout << "StackAllocator::allocate : An EXCEPTION occured! Exception #" << e << '\n';
	//}
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

unsigned int StackAllocator::padMemory(unsigned int sizeBytes) {
	unsigned int paddedMemory = sizeBytes % ARCH_BYTESIZE;
	if (paddedMemory != 0)
		paddedMemory = sizeBytes + (ARCH_BYTESIZE - paddedMemory);
	else
		paddedMemory = sizeBytes;
	return paddedMemory;
}