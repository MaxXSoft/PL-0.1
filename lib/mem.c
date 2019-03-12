#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include <util/pool.h>

static const int kInitBufferSize = 32;

typedef struct ArrayProto {
    int *ptr;
    unsigned int buffer_size;
    unsigned int len;
} Array;

int newarray() {
    // allocate new array
    PoolUnit unit;
    unit.ptr = malloc(sizeof(Array));
    unit.size = sizeof(Array);
    // initialize array
    Array *arr = unit.ptr;
    arr->buffer_size = kInitBufferSize;
    arr->ptr = malloc(arr->buffer_size * sizeof(int));
    arr->len = 0;
    return PoolAllocaUnit(unit);
}

int freearray(int arr) {
    PoolUnit *unit = PoolAccessUnit(arr);
    assert(unit && unit->size == sizeof(Array));
    // destroy array
    Array *a = unit->ptr;
    free(a->ptr);
    // free unit
    PoolFreeUnit(arr);
    return 0;
}

int getarraypos(int arr, int pos) {
    PoolUnit *unit = PoolAccessUnit(arr);
    assert(unit && unit->size == sizeof(Array));
    Array *a = unit->ptr;
    return a->ptr[pos];
}

int setarraypos(int arr, int pos, int value) {
    PoolUnit *unit = PoolAccessUnit(arr);
    assert(unit && unit->size == sizeof(Array));
    Array *a = unit->ptr;
    a->ptr[pos] = value;
    return 0;
}

int arrayfront(int arr) {
    PoolUnit *unit = PoolAccessUnit(arr);
    assert(unit && unit->size == sizeof(Array));
    Array *a = unit->ptr;
    return a->ptr[0];
}

int arrayback(int arr) {
    PoolUnit *unit = PoolAccessUnit(arr);
    assert(unit && unit->size == sizeof(Array));
    Array *a = unit->ptr;
    return a->ptr[a->len - 1];
}

int arrayempty(int arr) {
    PoolUnit *unit = PoolAccessUnit(arr);
    assert(unit && unit->size == sizeof(Array));
    Array *a = unit->ptr;
    return a->len == 0;
}

int arraylen(int arr) {
    PoolUnit *unit = PoolAccessUnit(arr);
    assert(unit && unit->size == sizeof(Array));
    Array *a = unit->ptr;
    return a->len;
}

int arraycapacity(int arr) {
    PoolUnit *unit = PoolAccessUnit(arr);
    assert(unit && unit->size == sizeof(Array));
    Array *a = unit->ptr;
    return a->buffer_size;
}

int arrayclear(int arr) {
    PoolUnit *unit = PoolAccessUnit(arr);
    assert(unit && unit->size == sizeof(Array));
    Array *a = unit->ptr;
    return a->len = 0;
}

int arraypush(int arr, int value) {
    PoolUnit *unit = PoolAccessUnit(arr);
    assert(unit && unit->size == sizeof(Array));
    Array *a = unit->ptr;
    // increase length of array
    ++a->len;
    if (a->len > a->buffer_size) {
        // need to reallocate memory
        a->buffer_size *= 2;
        void *ptr = realloc(a->ptr, a->buffer_size);
        if (!ptr) {
            free(a->ptr);
            abort();
        }
        else {
            a->ptr = ptr;
        }
    }
    // put value into back of array
    a->ptr[a->len - 1] = value;
    return 0;
}

int arraypop(int arr) {
    PoolUnit *unit = PoolAccessUnit(arr);
    assert(unit && unit->size == sizeof(Array));
    Array *a = unit->ptr;
    return a->ptr[--a->len];
}

int arrayresize(int arr, int size) {
    PoolUnit *unit = PoolAccessUnit(arr);
    assert(unit && unit->size == sizeof(Array));
    Array *a = unit->ptr;
    // upadte length
    int old_len = a->len;
    a->len = size;
    // need to reallocate memory
    if (a->len > a->buffer_size) {
        a->buffer_size = a->len * 2;
        void *ptr = realloc(a->ptr, a->buffer_size);
        if (!ptr) {
            free(a->ptr);
            abort();
        }
        else {
            a->ptr = ptr;
        }
    }
    // initialize new memory
    if (size > old_len) {
        memset(a->ptr + old_len, 0, (size - old_len) * sizeof(int));
    }
    return 0;
}
