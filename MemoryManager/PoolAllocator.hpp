#pragma once

#include <deque>
#include <shared_mutex>
#include <atomic>
#include <vector>
#include <iostream>

#include "Allocator.h"

class PoolAllocator: private Allocator 
{	
public:
	typedef unsigned int ID;

	struct Entry {
		//std::shared_mutex mu; // Used to make sure that the resource isn't read while it's being modified
		std::atomic_bool used = ATOMIC_VAR_INIT(false); // Used for atomic compare and exchange
		//std::thread::id val = std::this_thread::get_id(); // Temporary for testing
	};

public:
	PoolAllocator(void* memPtr, unsigned int entrySize, unsigned int numEntries);
	virtual ~PoolAllocator();

	void* allocate(int quadrant);
	
	virtual void deallocateAll();

	void deallocateSingle(void* address);

	void printEntries() {
		for (unsigned int i = 0; i < m_numEntries; i++)
			//std::cout << m_entries[i]->val << std::endl;
			std::cout << (int)m_entries[i]->used << "\n";
	}

	void cleanUp();
		
private:
	unsigned int findFreeEntry(int quadrant);

private:
	// Vector can't be used in conjunction with objects that can't be moved/copied
	// https://stackoverflow.com/questions/37870731/resize-a-stdvectorstdatomic-bool-assigning-true-to-all-atomic-bools
	std::deque<std::unique_ptr<Entry>> m_entries;

	size_t m_entrySize;
	unsigned int m_numEntries;

	std::vector<void*> quadFreeAddress;
};