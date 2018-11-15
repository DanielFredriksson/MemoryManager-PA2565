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

public:
	MemoryManager();
	~MemoryManager();

	void addPool(size_t sizeBytes);
	void addStack(size_t sizeBytes);
};