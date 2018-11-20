#pragma once
class Allocator 
{

protected:
	void* m_memPtr;
	size_t m_sizeBytes;

public:
	Allocator(void* memPtr, size_t sizeBytes);
	virtual ~Allocator();

	virtual void* allocate(size_t bytes) = 0;
	virtual void deallocateAll() = 0;

};

