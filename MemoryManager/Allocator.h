#ifndef ALLOCATOR
#define ALLOCATOR

class Allocator 
{

protected:
	void* m_memPtr;
	size_t m_sizeBytes;


public:
	Allocator(void* memPtr, unsigned int sizeBytes);
	virtual ~Allocator();

	virtual void deallocateAll() = 0;
	unsigned int padMemory(unsigned int sizeBytes);

};

#endif //ALLOCATOR

