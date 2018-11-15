#include "MemoryManager.hpp"

void * MemoryManager::getMem(size_t sizeBytes)
{
	return malloc(sizeBytes);
}

MemoryManager::MemoryManager()
{

}
MemoryManager::~MemoryManager()
{

}

void MemoryManager::addPool(size_t sizeBytes)
{
	m_pools.insert(new PoolAllocator(getMem(sizeBytes), sizeBytes));
}

void MemoryManager::addStack(size_t sizeBytes)
{
	m_stacks.insert(new StackAllocator(getMem(sizeBytes), sizeBytes));
}

