#ifndef POOL_ALLOCATOR
#define POOL_ALLOCATOR

#include <deque>
#include <shared_mutex>
#include <atomic>
#include <iostream>

#include "Allocator.h"

class PoolAllocator: private Allocator 
{
private:
	unsigned int findFreeEntry();

private:
	// Vector can't be used in conjuction with objects that can't be moved/copied
	// https://stackoverflow.com/questions/37870731/resize-a-stdvectorstdatomic-bool-assigning-true-to-all-atomic-bools
	std::deque<std::atomic_bool> m_entries;

	size_t m_sizeEachEntry;
	unsigned int m_numEntries;

	std::shared_mutex m_mtx;


public:
	typedef unsigned int ID;

	struct Entry {
		//std::shared_mutex mu; // Used to make sure that the resource isn't read while it's being modified
		std::atomic_bool used = ATOMIC_VAR_INIT(false); // Used for atomic compare and exchange
	};

public:
	PoolAllocator(void* memPtr, unsigned int sizeBytesEachEntry, unsigned int numEntries);
	virtual ~PoolAllocator();

	void* allocate();
	
	virtual void deallocateAll();

	bool removeEntry(const ID id);

	void printEntries() {
		for (unsigned int i = 0; i < m_numEntries; i++);
			//std::cout << m_entries[i]->val << std::endl;
	}

};

#endif //POOL_ALLOCATOR