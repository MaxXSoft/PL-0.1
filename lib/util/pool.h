#ifndef PL01_LIB_UTIL_POOL_H_
#define PL01_LIB_UTIL_POOL_H_

#include <stdlib.h>

typedef struct PoolUnitProto {
    void *ptr;
    unsigned int size;
} PoolUnit;

typedef unsigned int PoolId;

void InitializePool();
int IsPoolInitialized();
PoolUnit *PoolAccessUnit(PoolId id);
PoolId PoolAllocaUnit(PoolUnit unit);
void PoolFreeUnit(PoolId id);

#endif // PL01_LIB_UTIL_POOL_H_
