#include "MemoryBlock.h"

MemoryBlock::MemoryBlock()
	: pNext(nullptr), refCount(0), pAlloc(nullptr), inPool(false)
{
}

MemoryBlock::~MemoryBlock()
{
}