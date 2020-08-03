#include "MemoryAlloc.h"
#include <cstdlib>
#include <exception>
#include "MemoryBlock.h"


MemoryAlloc::MemoryAlloc()
	:pMemory(nullptr), pHeader(nullptr), blockNum(0), blockSize(0)
{
}

MemoryAlloc::~MemoryAlloc()
{
	if (pMemory != nullptr)
	{
		::free(pMemory);
	}
}

void* MemoryAlloc::alloc(size_t size)
{
	if (pMemory == nullptr)
	{
		init();
	}
	MemoryBlock* pResult = nullptr;
	if (pHeader == nullptr)
	{
		pResult = static_cast<MemoryBlock*>(
			malloc(size + sizeof(MemoryBlock))
			);
		pResult->inPool = false;
		pResult->refCount = 1;
		pResult->pAlloc = this;
		pResult->pNext = nullptr;
	}
	else
	{
		pResult = pHeader;
		pHeader = pHeader->pNext;
		pResult->refCount = 1;
	}
	return static_cast<void*>(
		reinterpret_cast<char*>(pResult) + sizeof(MemoryBlock)
		);
}

void MemoryAlloc::free(void* p)
{
	MemoryBlock* pBlock = reinterpret_cast<MemoryBlock*>(
		static_cast<char*>(p) + sizeof(MemoryBlock)
		);
	if (--pBlock->refCount != 0)
	{
		// 后期可能需要共享内存，另做处理
		throw std::exception("被多次引用");
	}
	if (pBlock->inPool)
	{
		pBlock->pNext = pHeader;
		pHeader = pBlock;
	}
	else
	{
		::free(pBlock);
	}
}

void MemoryAlloc::init()
{
	if (pMemory != nullptr)
	{
		throw std::exception("内存池已经初始化\n");
	}
	size_t memorySize = blockSize * blockNum; // 计算内存池大小
	pMemory = malloc(memorySize); // 向系统申请内存
	pHeader = static_cast<MemoryBlock*> (pMemory);

	MemoryBlock* pLast = pHeader;
	for (size_t i = 1; i < blockNum; i++)
	{
		MemoryBlock* pTemp = reinterpret_cast<MemoryBlock*> (
			static_cast<char*>(pMemory) + (i * blockSize)
			);
		pTemp->inPool = true;
		pTemp->refCount = 0;
		pTemp->pAlloc = this;
		pLast->pNext = pTemp;
		pLast = pTemp;
	}
	pLast->pNext = nullptr;
}