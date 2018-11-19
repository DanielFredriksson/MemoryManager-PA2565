#include "MemoryManager.hpp"

void * MemoryManager::getMem(size_t sizeBytes)
{
	return calloc(1, sizeBytes);
}

MemoryManager::MemoryManager()
{

}
MemoryManager::~MemoryManager()
{

}

PoolAllocator* MemoryManager::addPool(unsigned int sizeBytesEachEntry, unsigned int numEntries)
{
	PoolAllocator* temp = new PoolAllocator(getMem(sizeBytesEachEntry * numEntries), sizeBytesEachEntry, numEntries);
	m_pools.push_back(temp);
	return m_pools.back();
}

StackAllocator* MemoryManager::addStack(unsigned int sizeBytes)
{
	m_stacks.push_back(new StackAllocator(getMem(sizeBytes), sizeBytes));
	return m_stacks.back();
}

