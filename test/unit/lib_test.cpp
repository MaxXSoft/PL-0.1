#include <test.h>

extern "C" {
#include <lib.h>
#include <util/pool.h>
}

void PoolTest() {
    unsigned int test_var = 0xcafebabe;
    TEST_EXPECT(0, IsPoolInitialized());
    TEST_EXPECT(static_cast<PoolUnit *>(nullptr), PoolAccessUnit(0));
    TEST_EXPECT(0U, PoolAllocaUnit({&test_var, sizeof(test_var)}));
    TEST_EXPECT(1, IsPoolInitialized());
    PoolUnit *unit = PoolAccessUnit(0);
    TEST_EXPECT(reinterpret_cast<void *>(&test_var), unit->ptr);
    TEST_EXPECT(static_cast<unsigned int>(sizeof(test_var)), unit->size);
    TEST_EXPECT(1U, PoolAllocaUnit({&test_var, sizeof(test_var)}));
    TEST_EXPECT(2U, PoolAllocaUnit({&test_var, sizeof(test_var)}));
    PoolFreeUnit(0);
    TEST_EXPECT(0U, PoolAllocaUnit({&test_var, sizeof(test_var)}));
    PoolFreeUnit(2);
    TEST_EXPECT(2U, PoolAllocaUnit({&test_var, sizeof(test_var)}));
    TEST_EXPECT(3U, PoolAllocaUnit({&test_var, sizeof(test_var)}));
    PoolId id;
    for (int i = 0; i < 40; ++i) {
        id = PoolAllocaUnit({&test_var, sizeof(test_var)});
    }
    unit = PoolAccessUnit(id);
    TEST_EXPECT(0xcafebabe, *reinterpret_cast<unsigned int *>(unit->ptr));
    TEST_EXPECT(static_cast<unsigned int>(sizeof(test_var)), unit->size);
    unit = PoolAccessUnit(id - 1);
    TEST_EXPECT(0xcafebabe, *reinterpret_cast<unsigned int *>(unit->ptr));
    TEST_EXPECT(static_cast<unsigned int>(sizeof(test_var)), unit->size);
}

void LibTest() {
    // mem
    int arr = newarray();
    TEST_EXPECT(1, arrayempty(arr));
    TEST_EXPECT(0, arraylen(arr));
    arraypush(arr, 10);
    TEST_EXPECT(0, arrayempty(arr));
    TEST_EXPECT(1, arraylen(arr));
    TEST_EXPECT(10, getarraypos(arr, 0));
    TEST_EXPECT(10, arrayfront(arr));
    TEST_EXPECT(10, arrayback(arr));
    TEST_EXPECT(10, getarraypos(arr, 0));
    setarraypos(arr, 0, 42);
    TEST_EXPECT(42, arrayfront(arr));
    TEST_EXPECT(42, arrayback(arr));
    for (int i = 1; i <= 100; ++i) {
        arraypush(arr, i);
    }
    TEST_EXPECT(101, arraylen(arr));
    TEST_EXPECT(100, arrayback(arr));
    TEST_EXPECT(true, arrayfront(arr) != arrayback(arr));
    int sum = 0;
    while (!arrayempty(arr)) {
        sum += arraypop(arr);
    }
    TEST_EXPECT(5050 + 42, sum);
    arrayresize(arr, 100);
    TEST_EXPECT(100, arraylen(arr));
    arrayclear(arr);
    TEST_EXPECT(1, arrayempty(arr));
    TEST_EXPECT(0, arraylen(arr));
    TEST_EXPECT(0, freearray(arr));
    // string
    int str = newstring(10);
    TEST_EXPECT(10, stringmemlen(str));
    for (int i = 0; i < 10; ++i) {
        setstringpos(str, i, 'a' + i);
    }
    TEST_EXPECT(10, stringlen(str));
    int s2 = stringadd(str, str);
    TEST_EXPECT(20, stringlen(s2));
    int s3 = newstring(0);
    TEST_EXPECT(s3, stringassign(s3, str));
    TEST_EXPECT(s3, stringconcat(s3, str));
    TEST_EXPECT(20, stringlen(s3));
    TEST_EXPECT(0, stringcompare(s2, s3));
    TEST_EXPECT(0, freestring(str));
    TEST_EXPECT(0, freestring(s2));
    TEST_EXPECT(0, freestring(s3));
    str = newstring(3);
    setstringpos(str, 0, '1');
    setstringpos(str, 1, '.');
    setstringpos(str, 2, '1');
    float r = 1.1f;
    TEST_EXPECT(*reinterpret_cast<int *>(&r), stringtoreal(str));
    setstringpos(str, 1, '\0');
    TEST_EXPECT(1, stringtoint(str));
    TEST_EXPECT(0, freestring(str));
    str = newstring(8);
    setstringpos(str, 0, '1');
    setstringpos(str, 1, '.');
    setstringpos(str, 2, '2');
    setstringpos(str, 3, '5');
    setstringpos(str, 4, '0');
    setstringpos(str, 5, '0');
    setstringpos(str, 6, '0');
    setstringpos(str, 7, '0');
    r = 1.25f;
    s2 = realtostring(*reinterpret_cast<int *>(&r));
    TEST_EXPECT(0, stringcompare(str, s2));
    TEST_EXPECT(0, freestring(s2));
    s2 = inttostring(12345678);
    for (int i = 0; i < 8; ++i) {
        setstringpos(str, i, i + '1');
    }
    TEST_EXPECT(0, stringcompare(str, s2));
    TEST_EXPECT(0, freestring(str));
    TEST_EXPECT(0, freestring(s2));
}
