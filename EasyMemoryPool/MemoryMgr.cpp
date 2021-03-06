#include "MemoryMgr.h"
#include "MemoryBlock.h"
#include <cstdlib>
#include <cassert>
static size_t log2_to_int(size_t digit)
{
// 	return static_cast<size_t>(
// 		log2l(static_cast<long double>(digit))
// 		);
	int result = 0;
	while (digit >>= 1)
	{
		result++;
	}
	return result;
}

MemoryMgr* MemoryMgr::instance = nullptr;

MemoryMgr::MemoryMgr(
	size_t _MinBlockSize,
	size_t _MaxBlockSize,
	size_t _PoolBlocksNum)
	: minBlockSize(_MinBlockSize), maxBlockSize(_MaxBlockSize), poolBlocksNum(_PoolBlocksNum)
{
	assert(_MinBlockSize < _MaxBlockSize);
	if ((minBlockSize & minBlockSize - 1) != 0)
	{
		minBlockSize = 1ULL << (log2_to_int(_MinBlockSize) + 1);
	}
	if ((maxBlockSize & maxBlockSize - 1) != 0)
	{
		maxBlockSize = 1ULL << (log2_to_int(_MaxBlockSize) + 1);
	}
}

MemoryMgr::~MemoryMgr()
{
	for (MemoryAlloc* pAlloc : pools)
	{
		delete pAlloc;
	}
}

void* MemoryMgr::alloc(size_t size)
{
	if (size <= maxBlockSize)
	{
		if (pools.empty())
		{
			init();
		}
		return pools[getPoolIndex(size)]->alloc(size);
	}
	else
	{
		MemoryBlock* pResult = static_cast<MemoryBlock*>(
			malloc(size + sizeof(MemoryBlock))
			);
		pResult->inPool = false;
		pResult->refCount = 1;
		pResult->pAlloc = nullptr;
		pResult->pNext = nullptr;
		return  static_cast<void*>(reinterpret_cast<char*> (pResult) + sizeof(MemoryBlock));
	}
}

void MemoryMgr::free(void* p)
{
	MemoryBlock* pFree = reinterpret_cast<MemoryBlock*>(
		static_cast<char*>(p) - sizeof(MemoryAlloc)
		);
	if (--pFree->refCount != 0)
	{
		// 后期可能需要共享内存，另做处理
		throw std::exception("被多次引用");
	}
	if (pFree->inPool)
	{
		pFree->pAlloc->free(p);
	}
	else
	{
		::free(pFree);
	}
}

MemoryMgr* MemoryMgr::getInstance()
{
	if (instance == nullptr)
	{
		instance = new MemoryMgr();
	}
	return instance;
}

void MemoryMgr::init()
{
	for (size_t sz = minBlockSize; sz <= maxBlockSize; sz <<= 1)
	{
		pools.push_back(new MemoryAlloc(sz, poolBlocksNum));
	}
}

size_t MemoryMgr::getPoolIndex(size_t size)
{
	if (size <= minBlockSize)
	{
		return 0;
	}
	size_t index = log2_to_int(size);
	if ((size & size - 1) != 0)
	{
		index++;
	}
	index -= log2_to_int(minBlockSize);
	return index;
}
