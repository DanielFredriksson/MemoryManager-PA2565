#include "Allocator.h"


Allocator::Allocator(void* memPtr, size_t sizeBytes)
{
	m_memPtr = memPtr;
	m_sizeBytes = sizeBytes;
}


Allocator::~Allocator() 
{
}
