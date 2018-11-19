#pragma once

#include <vector>

#include "PoolAllocator.hpp"
#include "StackAllocator.hpp"

class MemoryManager 
{
private:
	std::vector<PoolAllocator*> m_pools;
	std::vector<StackAllocator*> m_stacks;

private:
	// GET DA MEMorIH
	void* getMem(size_t sizeBytes);

	// Singleton class shouldn't be able to be copied
	MemoryManager(MemoryManager const&) = delete;
	void operator=(MemoryManager const&) = delete;

public:
	MemoryManager();
	~MemoryManager();

	static MemoryManager& getInstance() {
		static MemoryManager instance;

		return instance;
	}

	PoolAllocator* addPool(size_t sizeBytesEachEntry, unsigned int numEntries);
	StackAllocator* addStack(size_t sizeBytes);
};