#include <lib/util/pool.h>

const int kPoolInitSize = 32;

typedef struct PoolIdListProto {
    PoolId id;
    struct PoolIdListProto *next;
} PoolIdList;

static PoolUnit *pool = NULL;
static PoolIdList *freed_id_list = NULL;
static PoolId pool_size = 0, next_id = 0;

void InitializePool() {
    // free allocated memory of pool
    if (pool) free(pool);
    PoolIdList *temp, *ptr = freed_id_list;
    while (ptr) {
        temp = ptr;
        ptr = ptr->next;
        free(temp);
    }
    // allocate new memory for pool
    pool_size = kPoolInitSize;
    pool = (PoolUnit *)malloc(pool_size * sizeof(PoolUnit));
    next_id = 0;
    freed_id_list = NULL;
}

int IsPoolInitialized() {
    return pool != NULL;
}

PoolUnit *PoolAccessUnit(PoolId id) {
    return id < next_id ? pool + id : NULL;
}

PoolId PoolAllocaUnit(PoolUnit unit) {
    if (freed_id_list) {
        // reuse freed id
        PoolId new_id = freed_id_list->id;
        PoolIdList *ptr = freed_id_list;
        freed_id_list = freed_id_list->next;
        free(ptr);
        pool[new_id] = unit;
        return new_id;
    }
    else {
        if (!IsPoolInitialized()) {
            InitializePool();
        }
        else if (next_id >= pool_size) {
            // resize pool
            pool_size *= 2;
            PoolUnit *p;
            p = (PoolUnit *)realloc(pool, pool_size * sizeof(PoolUnit));
            // abort when 'realloc' failure
            if (!p) {
                free(pool);
                abort();
            }
            else {
                pool = p;
            }
        }
        // set data
        pool[next_id] = unit;
        return next_id++;
    }
}

// NOTE: unsafe when free an id twice
void PoolFreeUnit(PoolId id) {
    // insert id info 'freed id' linked list
    PoolIdList *ptr = (PoolIdList *)malloc(sizeof(PoolIdList));
    ptr->id = id;
    ptr->next = freed_id_list;
    freed_id_list = ptr;
}
