#include "PoolAllocator.hpp"

/*
	Private Functions
*/
unsigned int PoolAllocator::findFreeEntry()
{
	for (unsigned int i = 0; i < m_entries.size(); i++) {
		bool expected = false;
		// Atomic compare and exchange, returns true if 'used == expected', else puts whatever 
		// 'used' was into 'expected' and tries again
		//if (m_entries[i]->used.compare_exchange_strong(expected, true))
			//return i;
	}
	return -1;
}

/*
	Public Functions
*/

PoolAllocator::PoolAllocator(void* memPtr, unsigned int sizeBytesEachEntry, unsigned int numEntries) 
	: Allocator(memPtr, sizeBytesEachEntry * numEntries)
{
	m_sizeEachEntry = sizeBytesEachEntry;
	m_numEntries = numEntries;

	for (unsigned int i = 0; i < numEntries; i++)
		m_entries.emplace_back(false);
}

PoolAllocator::~PoolAllocator()
{
	//this->cleanUp();
}

void * PoolAllocator::allocate()
{
	// Locks access to allocation while being deallocated
	std::shared_lock<std::shared_mutex> lock_guard(m_mtx);
	unsigned int index = PoolAllocator::findFreeEntry();
	if (index == -1) {
		// GET SOME MOH MEM BICH
		return nullptr;
	}
	else {
		// Makes sure any access to this entry in the memory is unaccessible by other threads
		//std::unique_lock<std::shared_mutex> lock_guard(m_entries[index]->mu);

		//m_entries[index]->val = std::this_thread::get_id();
		sizeof(Entry);

		// Return memory adress for the entry (memory_bottom_pointer + entry * size_of_entry)
		return static_cast<char*>(m_memPtr) + index * m_sizeEachEntry;
	}
}

void PoolAllocator::deallocateAll()
{
	// Can create problems if user doesn't "forget" the
	//  pointers to these 'blocks'
	std::unique_lock<std::shared_mutex> lock_guard(m_mtx);
	for (auto& i : m_entries)
		i = false;
}

bool PoolAllocator::removeEntry(const ID id)
{
	return false;
}

/*void PoolAllocator::cleanUp()
{
	if (this->m_memPtr != nullptr)
	{
		free(this->m_memPtr);
		this->m_memPtr = nullptr;
	}
}*/