#include "PoolAllocator.hpp"

/*
	Private Functions
*/
unsigned int PoolAllocator::findFreeEntry(int quadrent)
{
	char* tempAddress;

	tempAddress = (char*)this->m_memPtr;
	tempAddress += (int)((float)quadrent * ((float)this->m_sizeBytes * 0.25f));
	
	void* startAddress = (void*)tempAddress;
	void* stopAddress = (void*)(tempAddress += (int)((float)this->m_sizeBytes * 0.25f));
	void* allocationAddress = this->quadFreeAddress.at(quadrent);
	
	int entryNum = ((char*)(this->m_memPtr) - (char*)(allocationAddress));
	this->m_entries.at(entryNum)->used = true;

	tempAddress = (char*)this->quadFreeAddress.at(quadrent);
	while (this->m_entries.at(entryNum)->used == true)
	{
		tempAddress++;
		entryNum++;

		// If reached quadrant end; search from quadrant start
		if (tempAddress == stopAddress)
			tempAddress = (char*)startAddress;
		// If reaching where we started = quadrant is completely full!
		if (tempAddress == allocationAddress)
		{
			this->quadFreeAddress.at(quadrent) = nullptr;
			break;
		}
	}

	//for (unsigned int i = 0; i < m_entries.size(); i++) {
	//	bool expected = false;
	//	// Atomic compare and exchange, returns true if 'used == expected', else puts whatever 
	//	// 'used' was into 'expected' and tries again
	//	if (m_entries[i]->used.compare_exchange_strong(expected, true))
	//		return i;
	//}
	return -1;
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
		m_entries.emplace_back(std::unique_ptr<Entry>(new Entry()));
}

PoolAllocator::~PoolAllocator()
{
	this->cleanUp();
}

void * PoolAllocator::allocate(int quadrant)
{
	unsigned int index = PoolAllocator::findFreeEntry(quadrant);

	if (index == -1) {
		// GET SOME MOH MEM BICH
		return nullptr;
	}
	else {
		// Makes sure any access to this entry in the memory is unaccessible by other threads
		std::unique_lock<std::shared_mutex> lock_guard(m_entries[index]->mu);

		m_entries[index]->val = std::this_thread::get_id();

		// Return memory adress for the entry (memory_bottom_pointer + entry * size_of_entry)
		return static_cast<char*>(m_memPtr) + index * m_entrySize;
	}
}

void PoolAllocator::deallocateAll()
{
	for (auto& i : this->m_entries)
		i->used = false;

	// Set all new freeEntries (each quadrant)
}

void PoolAllocator::deallocateSingle(void* address)
{	// STEP 1
	char* startPoint;
	char* endPoint;

	startPoint = (char*)this->m_memPtr;
	endPoint = (char*)address;

	int entryIndex = (endPoint - startPoint);
	this->m_entries.at(entryIndex)->used = false;

	// STEP 2
	int quadrantSize = (int)((float)this->m_numEntries * 0.25f);
	int currentQuadrant = (entryIndex / quadrantSize);

	if (this->quadFreeAddress.at(currentQuadrant) == nullptr)
		this->quadFreeAddress.at(currentQuadrant) = address;
}

void PoolAllocator::cleanUp()
{
	if (this->m_memPtr != nullptr)
	{
		free(this->m_memPtr);
		this->m_memPtr = nullptr;
	}
}