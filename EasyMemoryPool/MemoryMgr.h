#ifndef _MEMORYMGR_H_
#define _MEMORYMGR_H_
#include "MemoryAlloc.h"
#include <vector>
// 每个内存池的内存块数量
#ifndef POOL_BLOCKS_NUM
#define POOL_BLOCKS_NUM 10U
#endif

// 所需最大的内存块的最小大小
#ifndef MAX_BLOCK_SIZE
#define MAX_BLOCK_SIZE 1024U
#endif

// 最小的内存块的大小
#ifndef MIN_BLOCK_SIZE
#define MIN_BLOCK_SIZE 64U
#endif

class MemoryMgr
{
	MemoryMgr(size_t _MinBlockSize = MIN_BLOCK_SIZE,
			  size_t _MaxBlockSize = MAX_BLOCK_SIZE,
			  size_t _PoolBlocksNum = POOL_BLOCKS_NUM);
public:
	~MemoryMgr();
	void* alloc(size_t size);
	void free(void* p);
	void init();
	static MemoryMgr* getInstance();
	size_t getPoolIndex(size_t size);
private:
	size_t maxBlockSize;
	size_t minBlockSize;
	size_t poolBlocksNum;
	std::vector<MemoryAlloc> pools;
	static MemoryMgr* instance;
};


#endif // !_MEMORYMGR_H_