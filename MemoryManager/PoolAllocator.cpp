#include "PoolAllocator.hpp"

unsigned int PoolAllocator::findFreeEntry()
{
	for (unsigned int i = 0; i < entries.size(); i++) {
		bool expected = false;
		// Atomic compare and exchange, returns true if 'used == expected', else puts whatever 'used' was 
		//	into 'expected' and tries again
		if (entries[i]->used.compare_exchange_strong(expected, true))
			return i;
	}
	return -1;
}

size_t PoolAllocator::space(Entry first, Entry second)
{
	return size_t();
}

PoolAllocator::PoolAllocator(void * memPtr, size_t sizeBytes) : Allocator(memPtr, sizeBytes)
{
	
}

PoolAllocator::~PoolAllocator()
{

}

void * PoolAllocator::allocate(size_t sizeBytes)
{
	unsigned int index = PoolAllocator::findFreeEntry();
	if (index == -1) {
		// GET SOME MOH MEM BICH
		return nullptr;
	}
	else {
		// Makes sure any access to this entry in the memory is unaccessible by other threads
		std::unique_lock<std::shared_mutex> lock_guard(entries[index]->mu);

		entries[index]->val = std::this_thread::get_id();

		// Return memory adress for the entry (memory_bottom_pointer + entry * size_of_entry)
		return 0;
	}
}

void PoolAllocator::deallocateAll()
{
}

bool PoolAllocator::removeEntry(const ID id)
{
	return false;
}
