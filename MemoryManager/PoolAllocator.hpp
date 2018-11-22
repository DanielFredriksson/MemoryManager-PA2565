#ifndef POOL_ALLOCATOR
#define POOL_ALLOCATOR

#include <deque>
#include <shared_mutex>
#include <atomic>
#include <vector>
#include <iostream>

#include "Allocator.h"

class PoolAllocator: private Allocator 
{

private:
	int findFreeEntry(int quadrant);

private:
	// Vector can't be used in conjunction with objects that can't be moved/copied
	// https://stackoverflow.com/questions/37870731/resize-a-stdvectorstdatomic-bool-assigning-true-to-all-atomic-bools
	std::deque<bool> m_entries;

	size_t m_entrySize;
	unsigned int m_numEntries;
	unsigned int m_numQuadrants;

	std::shared_mutex m_mtx;
	std::deque<std::atomic_bool> m_usedQuadrants;

	std::vector<void*> m_quadFreeAddress;

public:
	typedef unsigned int ID;

public:
	PoolAllocator(void* memPtr, unsigned int entrySize, unsigned int numEntries, unsigned int numQuadrants);
	virtual ~PoolAllocator();

	void* allocate(int quadrant);
	
	virtual void deallocateAll();

	void deallocateSingle(void* address);

	void printEntries() {
		for (unsigned int i = 0; i < m_numEntries; i++)
			//std::cout << m_entries[i]->val << std::endl;
			std::cout << (int)m_entries[i] << "\n";
	}

	// Memory tracking for debugging purposes
	virtual std::vector<bool> getUsedMemory();
	
	unsigned int getEntrySize() const;

	void cleanUp();
};

#endif //POOL_ALLOCATOR