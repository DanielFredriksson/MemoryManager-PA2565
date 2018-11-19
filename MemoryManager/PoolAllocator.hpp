#pragma once

#include <vector>
#include "Allocator.h"

typedef unsigned int ID;
class PoolAllocator: private Allocator 
{
	struct Entry;
private:
	std::vector<Entry> entries;

private:
	void* findHole(size_t byteSize);
	size_t space(Entry first, Entry second);
	
public:
	struct Entry {
		size_t byteSize;
		bool locked;
		void* memPtr; //Perhaps smartpointer (maaaabeh sharedptr)
	};

public:
	PoolAllocator(void* memPtr, size_t sizeBytes);
	virtual ~PoolAllocator();

	virtual void* allocate(size_t sizeBytes);
	virtual void deallocateAll();

	bool removeEntry(const ID id);
	void defrag();

};