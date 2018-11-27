#include "MemoryManager.hpp"

#include <string>

/*+-+-+-+-+-+-+-+-+-+-+-+
	PRIVATE FUNCTIONS
+-+-+-+-+-+-+-+-+-+-+-+*/

void* MemoryManager::getMem(unsigned int sizeBytes)
{
	return calloc(1, sizeBytes);
}

void MemoryManager::addPool(unsigned int sizeBytesEachEntry, unsigned int numEntries, unsigned int numQuadrants)
{
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
			return;
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





/*+-+-+-+-+-+-+-+-+-+-+-+
	 PUBLIC FUNCTIONS
+-+-+-+-+-+-+-+-+-+-+-+*/

MemoryManager::MemoryManager()
{
	m_stack = nullptr;
}
MemoryManager::~MemoryManager()
{
	cleanUp();
}

void MemoryManager::init(unsigned int stackSizeBytes, std::vector<PoolInstance> poolInstances)
{
	// Currently we only have one stack (single-frame stack)
	addStack(stackSizeBytes);
	m_currMemUsage.stacks = m_stack->getUsedMemory();

	int currIndex = 0;
	// Initializing each 'PoolInstance' into an actual pool in the memory manager
	for (PoolInstance PI : poolInstances) {
		if (PI.numEntries % PI.numQuadrants != 0)
			throw std::exception(("The number of entries in PoolInstance " + std::to_string(currIndex) + " was not divisible with the number of quadrants.").c_str());
		// Inserting the pool into the 'm_pools' vector
		addPool(PI.sizeBytesEachEntry, PI.numEntries, PI.numQuadrants);
		m_currMemUsage.pools.push_back(m_pools.at(currIndex++)->getUsedMemory());
	}
}

void* MemoryManager::singleFrameAllocate(unsigned int sizeBytes) {
	return m_stack->allocate(sizeBytes);
}

void* MemoryManager::randomAllocate(unsigned int sizeBytes) {
	void* ptr = nullptr;
	// Check for an appropriate sized pool
	for (auto it = m_pools.begin(); it != m_pools.end(); ++it) {
		if (sizeBytes <= (*it)->getEntrySize()) {
			ptr = (*it)->allocate();
			break;
		}
	}
	/*for (unsigned int i = 0; i < m_pools.size(); i++) {
		if (sizeBytes <= m_pools.at(i)->getEntrySize()) {
			ptr = m_pools.at(i)->allocate();
			i = m_pools.size();
		}
	}*/
	if (ptr == nullptr)
		throw std::exception("MemoryManager::randomAllocate : No pool allocated for size " + sizeBytes);

	return ptr;
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

void MemoryManager::updateAllocatedSpace()
{
	m_currMemUsage.stacks = m_stack->getUsedMemory();

	for (unsigned int i = 0; i < m_pools.size(); i++) {
		m_currMemUsage.pools.at(i) = m_pools.at(i)->getUsedMemory();
	}
}

MemoryUsage& MemoryManager::getAllocatedSpace()
{
	return m_currMemUsage;
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
	m_stack = nullptr;
}
