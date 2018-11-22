#ifndef MEMORY_MANAGER
#define MEMORY_MANAGER

#include <vector>

#include "PoolAllocator.hpp"
#include "StackAllocator.hpp"
#include <thread>

class MemoryManager
{
public:
	struct MemoryUsage {
		std::vector<bool> stacks;
		std::vector<std::vector<bool>> pools;
	};

	struct PoolInstance {
		unsigned int sizeBytesEachEntry;
		unsigned int numEntries;
		unsigned int numQuadrants;
	};

private:
	std::vector<PoolAllocator*> m_pools;
	StackAllocator* m_stack;
	bool m_threadsSet;

	MemoryUsage m_currMemUsage;

private:
	void* getMem(unsigned int sizeBytes);

	// Singleton class shouldn't be able to be copied
	MemoryManager(MemoryManager const&) = delete;
	void operator=(MemoryManager const&) = delete;

	std::vector<std::thread::id> m_threadIDs;

private:
	void addPool(unsigned int sizeBytesEachEntry, unsigned int numEntries, unsigned int numQuadrants);
	void addStack(unsigned int sizeBytes);

public:
	MemoryManager();
	~MemoryManager();

	static MemoryManager& getInstance() 
	{
		static MemoryManager instance;

		return instance;
	}

	void init(unsigned int stackSizeBytes, std::vector<PoolInstance> poolInstances);

	void* singleFrameAllocate(unsigned int sizeBytes);
	void* randomAllocate(unsigned int sizeBytes);

	void deallocateSingleRandom(void* ptr, unsigned int sizeOfAlloc);
	void deallocateAllRandom();
	void deallocateStack();

	void updateAllocatedSpace();
	MemoryUsage& getAllocatedSpace();

	void cleanUp();
};

#endif //MEMORY_MANAGER