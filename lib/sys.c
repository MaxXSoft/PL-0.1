#include <stdlib.h>
#include <time.h>

#ifdef __APPLE__
#include <crt_externs.h>
#endif

static int argc = -1;
static char **argv = NULL;

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
    argv = (char **)malloc(sizeof(char *) * argc);
    if (!argv) return -1;
    for (int i = 0; i < argc; ++i) {
        argv[i] = (*pargv)[i];
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
    if (argv || InitCommandLineArgs() >= 0) {
        return (int)argv[i];
    }
    else {
        return 0;
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
