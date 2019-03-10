#include <stdio.h>

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
    return printf("%s", str);
}

int println(int str) {
    return puts((char *)str);
}

// getchar
// putchar

int getfloat() {
    float f;
    return scanf("%f", &f) == EOF ? -1 : *(unsigned *)&f;
}

int putfloat(int f) {
    float f = *(float *)&f;
    return printf("%f", f);
}

int getstd(int fd) {
    switch (fd) {
        case 0: return (int)stdin;
        case 1: return (int)stdout;
        case 2: return (int)stderr;
        default: return -1;
    }
}

int flush(int file) {
    return fflush((FILE *)file);
}

int open(int filename, int mode) {
    return (int)fopen((char *)filename, (char *)mode);
}

int close(int file) {
    return fclose((FILE *)file);
}

int readfile(int file, int buf, int size, int count) {
    return fread((void *)buf, size, count, (FILE *)file);
}

int writefile(int file, int buf, int size, int count) {
    return fwrite((void *)buf, size, count, (FILE *)file);
}

int readchar(int file) {
    return fgetc((FILE *)file);
}

int writechar(int file, int c) {
    return fputc(c, (FILE *)file);
}

int readstring(int file, int str, int num) {
    return fgets((char *)str, num, (FILE *)file);
}

int writestring(int file, int str) {
    return fputs((char *)str, (FILE *)file);
}

int tell(int file) {
    return ftell((FILE *)file);
}

int seek(int file, int offset, int mode) {
    return fseek((FILE *)file, offset, mode);
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
