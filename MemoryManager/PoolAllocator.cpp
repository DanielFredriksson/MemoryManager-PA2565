#include "Defines.h"
#include "PoolAllocator.hpp"
/*
	Private Functions
*/

// 'quadrent' = Thread ID (integer between 0 and x, where x is the number of threads)
int PoolAllocator::findFreeEntry(int quadrant)
{
	int returnValue;

	void* allocationAddress = m_quadFreeAddress.at(quadrant);

	unsigned int entryNum = static_cast<char*>(allocationAddress) - static_cast<char*>(m_memPtr);
	entryNum = entryNum / m_entrySize;
	returnValue = entryNum;
	// Set that entry to 'allocated'
	m_entries.at(entryNum) = true;

	unsigned int startEntry = quadrant * m_entriesPerQuadrant;
	unsigned int entryNumOffset = entryNum - startEntry;

	m_quadFreeAddress.at(quadrant) = nullptr;
	// We are looking for the next free entry
	for (int i = 0; i < m_entriesPerQuadrant; i++) {
		entryNumOffset++;
		entryNumOffset %= m_entriesPerQuadrant;
		
		if (!m_entries.at(startEntry + entryNumOffset)) {
			m_quadFreeAddress.at(quadrant) = static_cast<char*>(m_memPtr) + (entryNumOffset + startEntry) * m_entrySize;
			break;
		}
	}
	
	// 'returnValue' = 'entryNum'
	return returnValue;
}

/*
	Public Functions
*/

PoolAllocator::PoolAllocator(void* memPtr, unsigned int entrySize, unsigned int numEntries, unsigned int numQuadrants) 
	: Allocator(memPtr, entrySize * numEntries)
{
	if (m_entrySize % ARCH_BYTESIZE != 0)
		throw std::exception("PoolAllocator::PoolAllocator(): Entry size was not a multiple of " + ARCH_BYTESIZE);

	m_entrySize = entrySize;
	m_numEntries = numEntries;
	m_numQuadrants = numQuadrants;
	m_entriesPerQuadrant = numEntries / numQuadrants;

	for (unsigned int i = 0; i < numEntries; i++)
		m_entries.emplace_back(false);

	m_usedQuadrants.resize(numQuadrants);
	for (unsigned int i = 0; i < m_usedQuadrants.size(); i++)
		m_usedQuadrants[i] = ATOMIC_VAR_INIT(false);

	// Set all new freeEntries (each quadrant)
	// Wrong calculation!! ERROR
	// int quadrantSize = static_cast<int>((static_cast<float>(m_numEntries) * 0.25f));
	int quadrantSize = static_cast<int>((static_cast<float>(m_sizeBytes) / float(m_numQuadrants)));

	for (int i = 0; i < m_numQuadrants; i++)
		m_quadFreeAddress.emplace_back(static_cast<char*>(m_memPtr) + (quadrantSize * i));
}

PoolAllocator::~PoolAllocator()
{
	cleanUp();
}

void* PoolAllocator::allocate()
{
	// Individual locks; used to check if a 'unique_lock' is currently in play
	// NOTE: 'unique_lock' in play = we are currently running 'deallocateAll()'
	//std::shared_lock<std::shared_mutex> lock(m_mtx); TEEHEEE
	// We're looking for a quadrant that's not being searched (= false)
	bool expected = false;
	int currentQuadrant = 0;
	// A returnValue of '-1' means the quadrant if completely full
	int entryReturnNum = -1;

	// Maximum amount of time allowed to try and allocate memory (subject to change)
	//std::chrono::system_clock::time_point sleepTill = std::chrono::system_clock::now() + std::chrono::milliseconds(100);
	
	while (entryReturnNum == -1)
	{
		// If the quadrant's 'm_usedQuadrants' == true, that means another thread is
		// searching in that quadrant already. So we look through the next
		// quadrant in the hopes that it's not being searched through.
		// also checks whether or not the quadrant has a free address
		while (!m_usedQuadrants.at(currentQuadrant).compare_exchange_strong(expected, true)
				|| m_quadFreeAddress.at(currentQuadrant) == nullptr)
		{

			// OPTI -- EATS A LOT OF CPU TIME
			// Throws if too much time have been taken during allocation
			//if (std::chrono::system_clock::now() > sleepTill)
			//	throw std::exception("All quadrants were full or in use for too long, initialize the pool with more memory.");
			
			
			// expected == true
			// at == true
			m_usedQuadrants.at(currentQuadrant) = false;
			expected = false;

			currentQuadrant++;
			currentQuadrant %= m_numQuadrants;
		}

		entryReturnNum = findFreeEntry(currentQuadrant);
		// Set the quadrant's 'm_usedMtxs' back to false for other threads
		m_usedQuadrants[currentQuadrant] = false;
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
	/// Wrong calculation!! ERROR
	// int quadrantSize = static_cast<int>((static_cast<float>(m_numEntries) * 0.25f));
	int quadrantSize = static_cast<int>((static_cast<float>(m_sizeBytes) / static_cast<float>(m_numQuadrants)));

	for (int i = 0; i < m_numQuadrants; i++)
		m_quadFreeAddress.at(i) = (static_cast<char*>(m_memPtr) + (quadrantSize * i));
}

void PoolAllocator::deallocateSingle(void* address)
{	/// STEP 1
	char* startPoint;
	char* endPoint;

	startPoint = static_cast<char*>(m_memPtr);
	endPoint = static_cast<char*>(address);
	// Calculates which entry we are deallocating 
	// FIX SIZE! ERROR!!!!
	int entryIndex = static_cast<int>(static_cast<float>((endPoint - startPoint) / static_cast<float>(m_entrySize)));
	// Setting the entry to false = deallocation
	m_entries.at(entryIndex) = false;

	/// STEP 2
	// Calculate the size of a quadrant
	int quadrantEntryCount = static_cast<int>(static_cast<float>(m_numEntries / float(m_numQuadrants)));
	// Check which quadrant we are in
	int currentQuadrant = static_cast<int>(static_cast<float>(entryIndex / quadrantEntryCount));
	// Set that specific quadrant's newest free entry to the one we
	// just deallocated.
	bool expected = false;
	// Wait for the quadrant to be free of other threads
	while (!m_usedQuadrants[currentQuadrant].compare_exchange_strong(expected, true))
		;//DO NOTHING
	m_quadFreeAddress.at(currentQuadrant) = address;
	m_usedQuadrants.at(currentQuadrant) = false;
}

std::vector<bool> PoolAllocator::getUsedMemory()
{
	std::vector<bool> usedMemory;
	for (unsigned int i = 0; i < m_entries.size(); i++)
		usedMemory.push_back(m_entries[i]);
	return usedMemory;
}

int PoolAllocator::getFreeEntriesCount()
{
	int returnValue = 0;

	for (unsigned int i = 0; i < m_entries.size(); i++)
		if (m_entries.at(i) == false)
			returnValue++;

	return returnValue;
}

unsigned int PoolAllocator::getEntrySize() const{
	return static_cast<unsigned int>(m_entrySize);
}

void PoolAllocator::cleanUp()
{
	if (m_memPtr != nullptr)
	{
		free(m_memPtr);
		m_memPtr = nullptr;
	}
}