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

PoolAllocator* MemoryManager::addPool(size_t sizeBytes)
{
	PoolAllocator* temp = new PoolAllocator(getMem(sizeBytes), sizeBytes);
	m_pools.insert(temp);
	return m_pools.back();
}

StackAllocator* MemoryManager::addStack(size_t sizeBytes)
{
	m_stacks.insert(new StackAllocator(getMem(sizeBytes), sizeBytes));
	return m_stacks.back();
}

