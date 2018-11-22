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
	size_t loopCount;

	loopCount = m_pools.size();
	for (unsigned int i = 0; i < loopCount; i++)
		if (m_pools.at(i) != nullptr)
			delete m_pools.at(i);
	m_pools.clear();
	m_pools.resize(0);

	loopCount = m_stacks.size();
	for (unsigned int i = 0; i < loopCount; i++)
		if (m_stacks.at(i) != nullptr)
			delete m_stacks.at(i);
	m_stacks.clear();
	m_stacks.resize(0);
}
