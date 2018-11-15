#include "PoolAllocator.hpp"

void * PoolAllocator::findHole(size_t byteSize)
{
	return nullptr;
}

size_t PoolAllocator::space(Entry first, Entry second)
{
	return size_t();
}

PoolAllocator::PoolAllocator(void * memPtr, size_t sizeBytes)
{
}

PoolAllocator::~PoolAllocator()
{

}

void * PoolAllocator::allocate(size_t sizeBytes)
{


	return nullptr;
}

void PoolAllocator::deallocateAll()
{
}

bool PoolAllocator::removeEntry(const ID id)
{
	return false;
}

void PoolAllocator::defrag()
{
}
