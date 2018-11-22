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

	// Check for pools with entires larger than the one being added
	for (it; it != m_pools.end(); it++) {
		if (sizeBytesEachEntry < (*it)->getEntrySize()) {
			// If found, insert it before it
			m_pools.insert(it, temp);
			largerFound = true;
		}
	}
	if (!largerFound) {
		// No pool found, add it to the back of the vector
		m_pools.push_back(temp);
	}
}

void MemoryManager::addStack(unsigned int sizeBytes)
{
	if (m_stack == nullptr)
		m_stack = new StackAllocator(getMem(sizeBytes), sizeBytes);
	else
		throw std::exception("MemoryManager::addStack : Stack already created");


}

void* MemoryManager::singleFrameAllocate(unsigned int sizeBytes) {
	return m_stack->allocate(sizeBytes);
}

void* MemoryManager::randomAllocate(unsigned int sizeBytes) {
	void* ptr = nullptr;
	// Check for an appropriate sized pool
	for (unsigned int i = 0; i < m_pools.size(); i++) {
		if (sizeBytes <= m_pools.at(i)->getEntrySize()) {
			ptr = m_pools.at(i)->allocate(getThreadID(std::this_thread::get_id()));
		}
	}
	if (ptr == nullptr)
		throw std::exception("MemoryManager::randomAllocate : No pool allocated for size " + sizeBytes);

	return ptr;

}

void MemoryManager::setThreads(std::vector<std::thread::id> threads) {
	m_threads.clear();
	for (unsigned int i = 0; i < threads.size(); i++) {
		m_threads.push_back(threads.at(i));
	}
}

unsigned int MemoryManager::getThreadID(std::thread::id id) const{

	for (unsigned int i = 0; i < m_threads.size(); i++) {
		if (id == m_threads.at(i))
			return i;
	}
	throw std::exception("MemoryManager::getThreadID(): Unmapped thread used");

	return -1;
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
