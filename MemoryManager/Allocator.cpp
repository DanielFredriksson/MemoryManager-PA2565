#include "Allocator.h"


Allocator::Allocator(void* memPtr, unsigned int sizeBytes)
{
	m_memPtr = memPtr;
	m_sizeBytes = padMemory(sizeBytes);
}


Allocator::~Allocator() 
{
}

unsigned int Allocator::padMemory(unsigned int sizeBytes) {
	unsigned int paddedMemory = sizeBytes % 8;
	paddedMemory = sizeBytes + (8 - paddedMemory);
	return paddedMemory;
}