#include "MemoryManager.hpp"

void* MemoryManager::getMem(unsigned int sizeBytes)
{
	return calloc(1, sizeBytes);
}

MemoryManager::MemoryManager()
{

}
MemoryManager::~MemoryManager()
{
	cleanUp();
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

	loopCount = m_pools.size();
	for (int i = 0; i < loopCount; i++)
		if (m_pools.at(loopCount) != nullptr)
			delete m_pools.at(loopCount);
	m_pools.clear();
	m_pools.resize(0);

	loopCount = m_stacks.size();
	for (int i = 0; i < loopCount; i++)
		if (m_stacks.at(loopCount) != nullptr)
			delete m_stacks.at(loopCount);
	m_stacks.clear();
	m_stacks.resize(0);
}
