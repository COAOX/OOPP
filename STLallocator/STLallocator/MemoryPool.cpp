#include "MemoryPool.h"
#include "stdafx.h"
#include <string>
#include <cstddef>

using namespace std;

class MemoryPool
{
private:
	constexpr static size_t ALIGN = 8;
	constexpr static size_t __MAX_BYTES = 128;
	constexpr static size_t NFreeLists = __MAX_BYTES / ALIGN;
private:
	void * m_freeLists[NFreeLists];
private:
	char* m_freeStart;
	char* m_freeEnd;
	size_t heapSize;
public:
	MemoryPool() :m_freeLists{nullptr},m_freeStart(nullptr),m_freeEnd(nullptr),heapSize(0){}
	
private:
	size_t roundUP(size_t _Count) {
		return (_Count + ALIGN - 1)&~(ALIGN - 1);
	}
	size_t freeLIstIndex(size_t _Count) {
		return (_Count + ALIGN - 1) / ALIGN - 1;
	}
private:
	void *chunkAlloc(size_t _Count, int &objs) {
		size_t needBytes = _Count*objs;
		size_t freeBytes = m_freeEnd-m_freeStart;
		void *res;
		if (needBytes <= freeBytes) {
			res = m_freeStart;
			m_freeStart += needBytes;
		}
		else if (_Count <= freeBytes) {
			objs = freeBytes / _Count;
			res = m_freeEnd;
			m_freeStart += _Count*objs;
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
	void* refill(size_t _Count) {
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
public:
	void *allocate(size_t _Count) {
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
	void deallocate(void *ptr, size_t _Count) {
		if (_Count > __MAX_BYTES) {
			::operator delete(ptr);
		}
		void **PtrFreeList = m_freeLists + freeLIstIndex(_Count);
		*reinterpret_cast<void **>(ptr) = *PtrFreeList;
		*PtrFreeList = ptr;
	}

};
