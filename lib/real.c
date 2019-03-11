#include <math.h>

#define GETF(i) (*(float *)&i)
#define GETI(f) (*(unsigned *)&f)

int inttoreal(int i) {
    float f = i;
    return GETI(f);
}

int realtoint(int r) {
    float f = GETF(r);
    return (int)f;
}

int realisnan(int r) {
    float f = GETF(r);
    return f != f;
}

int realadd(int l, int r) {
    float v = GETF(l) + GETF(r);
    return GETI(v);
}

int realsub(int l, int r) {
    float v = GETF(l) - GETF(r);
    return GETI(v);
}

int realmul(int l, int r) {
    float v = GETF(l) * GETF(r);
    return GETI(v);
}

int realdiv(int l, int r) {
    float v = GETF(l) / GETF(r);
    return GETI(v);
}

int realless(int l, int r) {
    float v = GETF(l) < GETF(r);
    return GETI(v);
}

int reallesseq(int l, int r) {
    float v = GETF(l) <= GETF(r);
    return GETI(v);
}

int realgreat(int l, int r) {
    float v = GETF(l) > GETF(r);
    return GETI(v);
}

int realgreateq(int l, int r) {
    float v = GETF(l) >= GETF(r);
    return GETI(v);
}

int realabs(int r) {
    float v = fabsf(GETF(r));
    return GETI(v);
}

int realmod(int l, int r) {
    float v = fmodf(GETF(l), GETF(r));
    return GETI(v);
}

int realpow(int l, int r) {
    float v = powf(GETF(l), GETF(r));
    return GETI(v);
}

int realsqrt(int r) {
    float v = sqrtf(GETF(r));
    return GETI(v);
}

int realmax(int l, int r) {
    float v = fmaxf(GETF(l), GETF(r));
    return GETI(v);
}

int realmin(int l, int r) {
    float v = fminf(GETF(l), GETF(r));
    return GETI(v);
}

int realexp(int r) {
    float v = expf(GETF(r));
    return GETI(v);
}

int realexp2(int r) {
    float v = exp2f(GETF(r));
    return GETI(v);
}

int reallog(int r) {
    float v = logf(GETF(r));
    return GETI(v);
}

int reallog10(int r) {
    float v = log10f(GETF(r));
    return GETI(v);
}

int reallog2(int r) {
    float v = log2f(GETF(r));
    return GETI(v);
}

int realsin(int r) {
    float v = sinf(GETF(r));
    return GETI(v);
}

int realcos(int r) {
    float v = cosf(GETF(r));
    return GETI(v);
}

int realtan(int r) {
    float v = tanf(GETF(r));
    return GETI(v);
}

int realasin(int r) {
    float v = asinf(GETF(r));
    return GETI(v);
}

int realacos(int r) {
    float v = acosf(GETF(r));
    return GETI(v);
}

int realatan(int r) {
    float v = atanf(GETF(r));
    return GETI(v);
}

int realsinh(int r) {
    float v = sinhf(GETF(r));
    return GETI(v);
}

int realcosh(int r) {
    float v = coshf(GETF(r));
    return GETI(v);
}

int realtanh(int r) {
    float v = tanhf(GETF(r));
    return GETI(v);
}

int realasinh(int r) {
    float v = asinhf(GETF(r));
    return GETI(v);
}

int realacosh(int r) {
    float v = acoshf(GETF(r));
    return GETI(v);
}

int realatanh(int r) {
    float v = atanhf(GETF(r));
    return GETI(v);
}

int realceil(int r) {
    float v = ceilf(GETF(r));
    return GETI(v);
}

int realfloor(int r) {
    float v = floorf(GETF(r));
    return GETI(v);
}
