#include "Allocator.h"


Allocator::Allocator(void* memPtr, size_t sizeBytes)
{
	m_memPtr = memPtr;
	m_sizeBytes = sizeBytes % 8;
	m_sizeBytes = sizeBytes + (8 - m_sizeBytes);
}


Allocator::~Allocator() 
{
}
