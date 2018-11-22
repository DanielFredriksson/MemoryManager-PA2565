#include "PoolAllocator.hpp"
/*
	Private Functions
*/

// 'quadrent' = Thread ID (0, 1, 2, or 3)
int PoolAllocator::findFreeEntry(int quadrent)
{
	int returnValue;

	void* allocationAddress = m_quadFreeAddress.at(quadrent);
	// If true = quadrent is already fully allocated!
	// EARLY EXIT (-1)
	if (allocationAddress == nullptr)
		return -1;

	char* tempAddress;
	// Address of pool's start
	tempAddress = static_cast<char*>(m_memPtr);
	// Address of quadrent's start
	tempAddress += static_cast<int>(static_cast<float>(quadrent) * static_cast<float>(m_sizeBytes) * 0.25f);
	// Which specific pool entry we are looking at
	int entryNum = static_cast<int>((static_cast<char*>(allocationAddress) - static_cast<char*>(m_memPtr)) / sizeof(char));
	returnValue = entryNum;
	// Set that entry to 'allocated'
	m_entries.at(entryNum) = true;

	tempAddress = static_cast<char*>(m_quadFreeAddress.at(quadrent));

	void* startAddress = static_cast<void*>(tempAddress);
	void* stopAddress = static_cast<void*>(tempAddress += static_cast<int>(static_cast<float>(m_sizeBytes) * 0.25f));
	// We are looking for the next free entry
	while (m_entries.at(entryNum) == true)
	{
		tempAddress += m_entrySize;
		entryNum++;

		// If reached quadrant end...
		if (tempAddress == stopAddress)
			// ... search from quadrant start
			tempAddress = static_cast<char*>(startAddress);
		// If reaching where we started = quadrant is completely full!
		if (tempAddress == allocationAddress)
		{	// '== nullptr' means that the memory is full.
			m_quadFreeAddress.at(quadrent) = nullptr;
		}
	}
	// 'returnValue' = 'entryNum'
	return returnValue;
}

/*
	Public Functions
*/

PoolAllocator::PoolAllocator(void* memPtr, unsigned int entrySize, unsigned int numEntries) 
	: Allocator(memPtr, entrySize * numEntries)
{
	m_entrySize = entrySize;
	m_numEntries = numEntries;

	for (unsigned int i = 0; i < numEntries; i++)
		m_entries.emplace_back(false);

	m_usedMtxs.resize(4);
	for (int i = 0; i < m_usedMtxs.size(); i++)
		m_usedMtxs[i] = ATOMIC_VAR_INIT(false);
}

PoolAllocator::~PoolAllocator()
{
	cleanUp();
}

void* PoolAllocator::allocate(int quadrant)
{
	// Individual locks; used to check if a 'unique_lock' is currently in play
	// NOTE: 'unique_lock' in play = we are currently running 'deallocateAll()'
	std::shared_lock<std::shared_mutex> lock(m_mtx);
	// We're looking for a quadrant that's not being searched (= false)
	bool expected = false;
	int currentQuadrant = quadrant;
	// A returnValue of '-1' means the quadrant if completely full
	int entryReturnNum = -1;

	while (entryReturnNum == -1)
	{	// If the quadrant's 'm_usedMtxs' == true, that means another thread is
		// searching in that quadrant already. So we look through the next
		// quadrant in the hopes that it's not being searched through.
		while (!m_usedMtxs.at(currentQuadrant).compare_exchange_strong(expected, true))
		{
			currentQuadrant++;
			if (currentQuadrant > 3)
				currentQuadrant = 0;
		}

		entryReturnNum = findFreeEntry(currentQuadrant);
		// Set the quadrant's 'm_usedMtxs' back to false for other threads
		m_usedMtxs[currentQuadrant] = ATOMIC_VAR_INIT(false);
	}
	// Calculating which address we have allocated, casting it to a void*
	char* returnAddress = static_cast<char*>(m_memPtr);
	returnAddress += (entryReturnNum * m_entrySize);
	return static_cast<void*>(returnAddress);
}

void PoolAllocator::deallocateAll()
{
	std::unique_lock<std::shared_mutex> lock(m_mtx);

	for (auto& i : m_entries)
		i = false;

	// Set all new freeEntries (each quadrant)
	int quadrantSize = (int)((float)m_numEntries * 0.25f);
	
	for (int i = 0; i < 4; i++)
		m_quadFreeAddress.at(i) = (static_cast<char*>(m_memPtr) + (quadrantSize * i));
}

void PoolAllocator::deallocateSingle(void* address)
{	/// STEP 1
	char* startPoint;
	char* endPoint;

	startPoint = static_cast<char*>(m_memPtr);
	endPoint = static_cast<char*>(address);
	// Calculates which entry we are deallocating
	int entryIndex = static_cast<int>(static_cast<float>((endPoint - startPoint) / static_cast<float>(sizeof(char))));
	// Setting the entry to false = deallocation
	m_entries.at(entryIndex) = false;

	/// STEP 2
	// Calculate the size of a quadrant
	int quadrantSize = static_cast<int>(static_cast<float>(m_numEntries * 0.25f));
	// Check which quadrant we are in
	int currentQuadrant = static_cast<int>(static_cast<float>(entryIndex / quadrantSize));
	// Set that specific quadrant's newest free entry to the one we
	// just deallocated.
	bool expected = false;
	// Wait for the quadrant to be free of other threads
	while (!m_usedMtxs[currentQuadrant].compare_exchange_strong(expected, true))
		;//DO NOTHING
	m_quadFreeAddress.at(currentQuadrant) = address;
	m_usedMtxs.at(currentQuadrant) = ATOMIC_VAR_INIT(false);
}

void PoolAllocator::cleanUp()
{
	if (m_memPtr != nullptr)
	{
		free(m_memPtr);
		m_memPtr = nullptr;
	}
}