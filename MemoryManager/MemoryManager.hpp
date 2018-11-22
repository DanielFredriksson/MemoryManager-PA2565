#ifndef MEMORY_MANAGER
#define MEMORY_MANAGER

#include <vector>

#include "PoolAllocator.hpp"
#include "StackAllocator.hpp"
#include <thread>

class MemoryManager
{
private:
	std::vector<PoolAllocator*> m_pools;
	StackAllocator* m_stack;
	bool m_threadsSet;

private:
	// GET DA MEMorIH
	void* getMem(unsigned int sizeBytes);

	// Singleton class shouldn't be able to be copied
	MemoryManager(MemoryManager const&) = delete;
	void operator=(MemoryManager const&) = delete;

	std::vector<std::thread::id> m_threadIDs;

public:
	MemoryManager();
	~MemoryManager();

	static MemoryManager& getInstance() 
	{
		static MemoryManager instance;

		return instance;
	}

	void addPool(unsigned int sizeBytesEachEntry, unsigned int numEntries);
	void addStack(unsigned int sizeBytes);

	void* singleFrameAllocate(unsigned int sizeBytes);
	void* randomAllocate(unsigned int sizeBytes);

	void setThreads(std::vector<std::thread::id> threads);
	unsigned int getThreadID(std::thread::id id) const;

	void deallocateSingleRandom(void* ptr, unsigned int sizeOfAlloc);
	void deallocateAllRandom();
	void deallocateStack();

	void cleanUp();
};

#endif //MEMORY_MANAGER