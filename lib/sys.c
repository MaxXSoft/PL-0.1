#include <stdlib.h>
#include <time.h>
#include <string.h>

#ifdef __APPLE__
#include <crt_externs.h>
#endif

#include <lib/util/pool.h>

static int argc = -1;
static PoolId *p_argv = NULL;

static int InitCommandLineArgs() {
#ifdef __APPLE__
    // get pointer of argc
    int *pargc = _NSGetArgc();
    if (!pargc) return -1;
    argc = *pargc;
    // get pointer of argv
    char ***pargv = _NSGetArgv();
    if (!pargv) return -1;
    // allocate spaces
    p_argv = (PoolId *)malloc(argc * sizeof(PoolId));
    if (!p_argv) return -1;
    PoolUnit unit;
    for (int i = 0; i < argc; ++i) {
        unit.ptr = (*pargv)[i];
        unit.size = strlen(unit.ptr);
        p_argv[i] = PoolAllocaUnit(unit);
    }
    return 0;
#else
    return -1;
#endif
}

int quit(int num) {
    exit(num);
    return 0;
}

int getargcount() {
    if (argc == -1) InitCommandLineArgs();
    return argc;
}

int getargvalue(int i) {
    if (p_argv || InitCommandLineArgs() >= 0) {
        return p_argv[i];
    }
    else {
        return -1;
    }
}

int randomize(int seed) {
    srand(seed);
    return 0;
}

int getrand() {
    return rand();
}

int timestamp() {
    return (int)time(NULL);
}
