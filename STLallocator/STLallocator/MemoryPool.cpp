#include "stdafx.h"
#include <string>
#include <cstddef>
#include <memory>
#include "MemoryPool.h"

//MemoryPool::MemoryPool() :m_freeLists{ nullptr }, m_freeStart(nullptr), m_freeEnd(nullptr), heapSize(0){}

size_t MemoryPool::roundUP(size_t _Count) {
	return (_Count + ALIGN - 1)&~(ALIGN - 1);
}
size_t MemoryPool::freeLIstIndex(size_t _Count) {
	return (_Count + ALIGN - 1) / ALIGN - 1;
}
void *MemoryPool::chunkAlloc(size_t _Count, int &objs) {
	size_t needBytes = _Count * objs;
	size_t freeBytes = m_freeEnd - m_freeStart;
	void *res;
	if (needBytes <= freeBytes) {
		res = m_freeStart;
		m_freeStart += needBytes;
	}
	else if (_Count <= freeBytes) {
		objs = freeBytes / _Count;
		res = m_freeEnd;
		m_freeStart += _Count * objs;
	}
	else {
		if (freeBytes > 0) {
			void **freeList = m_freeLists + freeLIstIndex(freeBytes);
			*reinterpret_cast<void **>(m_freeStart) = *freeList;
			m_freeStart += freeBytes;
		}
		size_t bytesToGet = 2 * needBytes + roundUP(heapSize >> 4);
		m_freeStart = reinterpret_cast<char *>(operator new(bytesToGet));
		heapSize += bytesToGet;
		m_freeEnd = m_freeStart + bytesToGet;
		return chunkAlloc(_Count, objs);
	}
	return res;
}
void* MemoryPool::refill(size_t _Count) {
	int objs = 20;
	void *Chunk = chunkAlloc(_Count, objs);
	if (objs > 1) {
		void ** FreeList = m_freeLists + freeLIstIndex(_Count);
		void *next = reinterpret_cast<char *>(Chunk) + _Count;
		*FreeList = next;
		for (int i = 2; i < objs; i++) {
			void **current = reinterpret_cast<void **>(next);
			next = reinterpret_cast<char *>(Chunk) + _Count;
			*current = next;
		}
		*reinterpret_cast<void **>(next) = nullptr;
	}
	return Chunk;
}
void *MemoryPool::allocate_pool(size_t _Count) {
	if (_Count > __MAX_BYTES) {
		return :: operator new(_Count);
	}
	void **PtrFreeList = m_freeLists + freeLIstIndex(_Count);
	void *result = *PtrFreeList;
	if (result == nullptr)
	{
		return refill(roundUP(_Count));
	}
	*PtrFreeList = *reinterpret_cast<void **>(result);
	return result;
}
void MemoryPool::deallocate_pool(void *ptr, size_t _Count) {
	if (_Count > __MAX_BYTES) {
		::operator delete (ptr);
	}
	void **PtrFreeList = m_freeLists + freeLIstIndex(_Count);
	*reinterpret_cast<void **>(ptr) = *PtrFreeList;
	*PtrFreeList = ptr;
}

	
