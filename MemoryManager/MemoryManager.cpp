#include "MemoryManager.hpp"

void* MemoryManager::getMem(unsigned int sizeBytes)
{
	return calloc(1, sizeBytes);
}

MemoryManager::MemoryManager()
{
	m_stack = nullptr;
}
MemoryManager::~MemoryManager()
{
	cleanUp();
}

void MemoryManager::addPool(unsigned int sizeBytesEachEntry, unsigned int numEntries)
{
	PoolAllocator* temp = new PoolAllocator(getMem(sizeBytesEachEntry * numEntries), sizeBytesEachEntry, numEntries);
	std::vector<PoolAllocator*>::iterator it = m_pools.begin();
	int pos = 0;
	bool largerFound = false;

	for (it; it != m_pools.end(); it++) {
		if (sizeBytesEachEntry < (*it)->getEntrySize()) {
			m_pools.insert(it, temp);
			largerFound = true;
		}
	}
	if (!largerFound) {
		m_pools.push_back(temp);
	}
}

void MemoryManager::addStack(unsigned int sizeBytes)
{
	if(m_stack == nullptr)
		m_stack = new StackAllocator(getMem(sizeBytes), sizeBytes);

}

void* MemoryManager::singleFrameAllocate(unsigned int sizeBytes) {
	return m_stack->allocate(sizeBytes);
}

void* MemoryManager::randomAllocate(unsigned int sizeBytes) {
	void* ptr = nullptr;
	for (unsigned int i = 0; i < m_pools.size(); i++) {
		if (sizeBytes < m_pools.at(i)->getEntrySize()) {
			//ptr = m_pools.at(i)->allocate();
		}

	}

	return ptr;
}

void MemoryManager::setThreads(std::vector<std::thread> threads) {
	
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

	delete m_stack;
}
