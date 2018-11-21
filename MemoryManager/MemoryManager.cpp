#include "MemoryManager.hpp"

void* MemoryManager::getMem(size_t sizeBytes)
{
	return calloc(1, sizeBytes);
}

MemoryManager::MemoryManager()
{

}
MemoryManager::~MemoryManager()
{
	this->cleanUp();
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

void MemoryManager::cleanUp()
{
	int loopCount;

	loopCount = this->m_pools.size();
	for (int i = 0; i < loopCount; i++)
		if (this->m_pools.at(i) != nullptr)
			delete this->m_pools.at(i);
	this->m_pools.clear();
	this->m_pools.resize(0);

	loopCount = this->m_stacks.size();
	for (int i = 0; i < loopCount; i++)
		if (this->m_stacks.at(i) != nullptr)
			delete this->m_stacks.at(i);
	this->m_stacks.clear();
	this->m_stacks.resize(0);
}
