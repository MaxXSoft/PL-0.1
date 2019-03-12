#include <stdio.h>
#include <assert.h>

#include <util/pool.h>

// standard I/O in global pool
static PoolId p_stdin = 0, p_stdout = 0, p_stderr = 0;

static void InitializeStdPtr() {
    PoolUnit unit;
    unit.size = sizeof(FILE);
    unit.ptr = stdin;
    p_stdin = PoolAllocaUnit(unit);
    unit.ptr = stdout;
    p_stdout = PoolAllocaUnit(unit);
    unit.ptr = stderr;
    p_stderr = PoolAllocaUnit(unit);
}

int read() {
    int i;
    return scanf("%d", &i) == EOF ? -1 : i;
}

int write(int i) {
    return printf("%d", i);
}

int writeln(int i) {
    return printf("%d\n", i);
}

int print(int str) {
    PoolUnit *unit = PoolAccessUnit(str);
    assert(unit);
    return printf("%s", unit->ptr);
}

int println(int str) {
    PoolUnit *unit = PoolAccessUnit(str);
    assert(unit);
    return puts((char *)unit->ptr);
}

// getchar
// putchar

int getreal() {
    float f;
    return scanf("%f", &f) == EOF ? -1 : *(unsigned *)&f;
}

int putreal(int r) {
    float f = *(float *)&r;
    return printf("%f", f);
}

int getstd(int fd) {
    if (p_stdin == p_stdout && p_stdout == p_stderr) InitializeStdPtr();
    switch (fd) {
        case 0: return p_stdin;
        case 1: return p_stdout;
        case 2: return p_stderr;
        default: return -1;
    }
}

int flush(int file) {
    PoolUnit *unit = PoolAccessUnit(file);
    assert(unit && unit->size == sizeof(FILE));
    return fflush((FILE *)unit->ptr);
}

int open(int filename, int mode) {
    PoolUnit *fn = PoolAccessUnit(filename), *md = PoolAccessUnit(mode);
    assert(fn && md);
    PoolUnit unit;
    unit.ptr = fopen((char *)fn->ptr, (char *)md->ptr);
    unit.size = sizeof(FILE);
    return PoolAllocaUnit(unit);
}

int close(int file) {
    PoolUnit *unit = PoolAccessUnit(file);
    assert(unit && unit->size == sizeof(FILE));
    return fclose((FILE *)unit->ptr);
}

int readfile(int file, int buf, int size, int count) {
    PoolUnit *f = PoolAccessUnit(file), *bf = PoolAccessUnit(buf);
    assert(f && f->size == sizeof(FILE) && bf);
    return fread(bf->ptr, size, count, (FILE *)f->ptr);
}

int writefile(int file, int buf, int size, int count) {
    PoolUnit *f = PoolAccessUnit(file), *bf = PoolAccessUnit(buf);
    assert(f && f->size == sizeof(FILE) && bf);
    return fwrite(bf->ptr, size, count, (FILE *)f->ptr);
}

int readchar(int file) {
    PoolUnit *unit = PoolAccessUnit(file);
    assert(unit && unit->size == sizeof(FILE));
    return fgetc((FILE *)unit->ptr);
}

int writechar(int file, int c) {
    PoolUnit *unit = PoolAccessUnit(file);
    assert(unit && unit->size == sizeof(FILE));
    return fputc(c, (FILE *)unit->ptr);
}

int readstring(int file, int str, int num) {
    PoolUnit *f = PoolAccessUnit(file), *s = PoolAccessUnit(str);
    assert(f && f->size == sizeof(FILE) && s);
    fgets((char *)s->ptr, num, (FILE *)f->ptr);
    return 0;
}

int writestring(int file, int str) {
    PoolUnit *f = PoolAccessUnit(file), *s = PoolAccessUnit(str);
    assert(f && f->size == sizeof(FILE) && s);
    return fputs((char *)s->ptr, (FILE *)f->ptr);
}

int tell(int file) {
    PoolUnit *unit = PoolAccessUnit(file);
    assert(unit && unit->size == sizeof(FILE));
    return ftell((FILE *)unit->ptr);
}

int seek(int file, int offset, int mode) {
    PoolUnit *unit = PoolAccessUnit(file);
    assert(unit && unit->size == sizeof(FILE));
    return fseek((FILE *)unit->ptr, offset, mode);
}

int and(int l, int r) {
    return l & r;
}

int or(int l, int r) {
    return l | r;
}

int not(int opr) {
    return ~opr;
}

int xor(int l, int r) {
    return l ^ r;
}

int shl(int l, int r) {
    return l << r;
}

int shr(int l, int r) {
    return l >> r;
}

int mod(int l, int r) {
    return l % r;
}

int logicand(int l, int r) {
    return l && r;
}

int logicor(int l, int r) {
    return l || r;
}

int logicnot(int opr) {
    return !opr;
}
