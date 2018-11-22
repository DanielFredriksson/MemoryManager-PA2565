#include "MemoryManager.hpp"

void* MemoryManager::getMem(unsigned int sizeBytes)
{
	return calloc(1, sizeBytes);
}

MemoryManager::MemoryManager()
{
	m_stack = nullptr;
	m_threadsSet = false;
}
MemoryManager::~MemoryManager()
{
	cleanUp();
}

void MemoryManager::addPool(unsigned int sizeBytesEachEntry, unsigned int numEntries)
{
	unsigned int numQuadrants = static_cast<unsigned int>(m_threadIDs.size());
	PoolAllocator* temp = new PoolAllocator(getMem(sizeBytesEachEntry * numEntries), sizeBytesEachEntry, numEntries, numQuadrants);
	std::vector<PoolAllocator*>::iterator it = m_pools.begin();
	int pos = 0;
	bool largerFound = false;

	// Check for pools with entires larger than the one being added
	for (it; it != m_pools.end(); it++) {
		if (sizeBytesEachEntry < (*it)->getEntrySize()) {
			// If found a pool with larger entries, insert the new pool before the larger pool
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
	if (!m_threadsSet) {
		for (unsigned int i = 0; i < threads.size(); i++)
			m_threadIDs.push_back(threads.at(i));
		m_threadsSet = true;
	}
}

unsigned int MemoryManager::getThreadID(std::thread::id id) const{

	for (unsigned int i = 0; i < m_threadIDs.size(); i++) {
		if (id == m_threadIDs.at(i))
			return i;
	}
	throw std::exception("MemoryManager::getThreadID(): Unmapped thread used");

	return -1;
}

void MemoryManager::deallocateSingleRandom(void* ptr, unsigned int sizeOfAlloc)
{
	for (unsigned int i = 0; i < m_pools.size(); i++)
		if (sizeOfAlloc <= m_pools.at(i)->getEntrySize())
			m_pools.at(i)->deallocateSingle(ptr);
}

void MemoryManager::deallocateAllRandom()
{
	for (unsigned int i = 0; i < m_pools.size(); i++)
		m_pools.at(i)->deallocateAll();
}

void MemoryManager::deallocateStack() {
	m_stack->deallocateAll();
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
