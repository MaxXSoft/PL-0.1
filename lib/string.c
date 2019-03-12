#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <lib/util/pool.h>

// NOTE: a stupid implementation of variable length string

int newstring(int size) {
    PoolUnit unit;
    unit.ptr = malloc((size + 1) * sizeof(char));   // '\0'
    unit.size = size;
    return PoolAllocaUnit(unit);
}

int freestring(int str) {
    PoolFreeUnit(str);
    return 0;
}

int getstringpos(int str, int pos) {
    PoolUnit *unit = PoolAccessUnit(str);
    assert(unit);
    return pos < unit->size ? ((char *)unit->ptr)[pos] : -1;
}

int setstringpos(int str, int pos, int c) {
    PoolUnit *unit = PoolAccessUnit(str);
    assert(unit);
    if (pos < unit->size) {
        ((char *)unit->size)[pos] = c;
        return 0;
    }
    else {
        return -1;
    }
}

int stringlen(int str) {
    PoolUnit *unit = PoolAccessUnit(str);
    assert(unit);
    return unit->size;
}

int stringadd(int str1, int str2) {
    PoolUnit *s1 = PoolAccessUnit(str1), *s2 = PoolAccessUnit(str2);
    assert(s1 && s2);
    // allocate memory for new string
    PoolUnit unit;
    unit.ptr = malloc((s1->size + s2->size + 1) * sizeof(char));
    unit.size = s1->size + s2->size;
    // copy string to new string
    memcpy(unit.ptr, s1->ptr, s1->size);
    strcpy(unit.ptr + s1->size, s2->ptr);
    return PoolAllocaUnit(unit);
}

int stringconcat(int dest, int src) {
    PoolUnit *s1 = PoolAccessUnit(dest), *s2 = PoolAccessUnit(src);
    assert(s1 && s2);
    // reallocate memory for new string
    void *mem = realloc(s1->ptr, (s1->size + s2->size + 1) * sizeof(char));
    if (!mem) {
        free(s1->ptr);
        abort();
    }
    else {
        s1->ptr = mem;
        s1->size += s2->size;
    }
    // string concatenation
    strcat(s1->ptr, s2->ptr);
    return dest;
}

int stringcompare(int str1, int str2) {
    PoolUnit *s1 = PoolAccessUnit(str1), *s2 = PoolAccessUnit(str2);
    assert(s1 && s2);
    return strcmp(s1->ptr, s2->ptr);
}

int stringassign(int dest, int src) {
    PoolUnit *s1 = PoolAccessUnit(dest), *s2 = PoolAccessUnit(src);
    assert(s1 && s2);
    // reallocate memory for new string
    void *mem = realloc(s1->ptr, (s2->size + 1) * sizeof(char));
    if (!mem) {
        free(s1->ptr);
        abort();
    }
    else {
        s1->ptr = mem;
        s1->size = s2->size;
    }
    // cpoy string
    strcpy(s1->ptr, s2->ptr);
    return dest;
}

int stringtoint(int str) {
    PoolUnit *s = PoolAccessUnit(str);
    assert(s);
    return atoi(s->ptr);
}

int inttostring(int i) {
    PoolUnit unit;
    unit.ptr = malloc(16 * sizeof(char));
    sprintf(unit.ptr, "%d", i);
    unit.size = strlen(unit.ptr);
    return PoolAllocaUnit(unit);
}

int stringtoreal(int str) {
    PoolUnit *s = PoolAccessUnit(str);
    assert(s);
    float r = atof(s->ptr);
    return *(unsigned *)&r;
}

int realtostring(int r) {
    PoolUnit unit;
    unit.ptr = malloc(64 * sizeof(char));
    sprintf(unit.ptr, "%f", *(float *)&r);
    unit.size = strlen(unit.ptr);
    return PoolAllocaUnit(unit);
}
