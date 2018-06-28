#pragma once

class MemoryPool
{
private:
	constexpr static size_t ALIGN = 8;
	constexpr static size_t __MAX_BYTES = 128;
	constexpr static size_t NFreeLists = __MAX_BYTES / ALIGN;
private:
	void * m_freeLists[NFreeLists];
private:
	char *m_freeStart;
	char *m_freeEnd;
	size_t heapSize;
public:
	MemoryPool() :m_freeLists{ nullptr }, m_freeStart(nullptr), m_freeEnd(nullptr), heapSize(0){}

private:
	size_t roundUP(size_t _Count);
	size_t freeLIstIndex(size_t _Count);
private:
	void *chunkAlloc(size_t _Count, int &objs);
	void* refill(size_t _Count);
public:
	void *allocate(size_t _Count);
	void deallocate(void *ptr, size_t _Count);

};