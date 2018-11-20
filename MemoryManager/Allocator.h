#ifndef ALLOCATOR
#define ALLOCATOR
class Allocator 
{

protected:
	void* m_memPtr;
	size_t m_sizeBytes;

public:
	Allocator(void* memPtr, size_t sizeBytes);
	virtual ~Allocator();

	virtual void deallocateAll() = 0;

};

#endif //ALLOCATOR

