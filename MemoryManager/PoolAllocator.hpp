#pragma once

#include <deque>
#include <shared_mutex>
#include <atomic>

#include "Allocator.h"

typedef unsigned int ID;
class PoolAllocator: private Allocator 
{
	struct Entry;
private:
	// Vector can't be used in conjuction with objects that can't be moved/copied
	// https://stackoverflow.com/questions/37870731/resize-a-stdvectorstdatomic-bool-assigning-true-to-all-atomic-bools
	std::deque<std::unique_ptr<Entry>> entries;

private:
	unsigned int findFreeEntry();
	size_t space(Entry first, Entry second);
	
public:
	struct Entry {
		std::shared_mutex mu; // Used to make sure that the resource isn't read while it's being modified
		std::atomic_bool used = ATOMIC_VAR_INIT(false); // Used for atomic compare and exchange
		std::thread::id val = std::this_thread::get_id(); // Temporary for testing
	};

public:
	PoolAllocator(void* memPtr, size_t sizeBytes);
	virtual ~PoolAllocator();

	virtual void* allocate(size_t sizeBytes);
	virtual void deallocateAll();

	bool removeEntry(const ID id);

};