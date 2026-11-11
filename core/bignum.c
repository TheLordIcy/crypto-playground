#include "bignum.h"

#include <stdlib.h>

bignum *bn_new(size_t nlimbs) {
    bignum *a = (bignum *)malloc(sizeof *a);
    if (a == NULL) {
        return NULL;
    }
    a->limbs = (uint32_t *)calloc(nlimbs ? nlimbs : 1, sizeof *a->limbs);
    if (a->limbs == NULL) {
        free(a);
        return NULL;
    }
    a->nlimbs = nlimbs ? nlimbs : 1;
    a->sign = 0;
    return a;
}

void bn_free(bignum *a) {
    if (a == NULL) {
        return;
    }
    free(a->limbs);
    free(a);
}

void bn_copy(bignum *dst, const bignum *src) {
    if (dst == NULL || src == NULL || dst->nlimbs < src->nlimbs) {
        return;
    }
    for (size_t i = 0; i < src->nlimbs; i++) {
        dst->limbs[i] = src->limbs[i];
    }
    for (size_t i = src->nlimbs; i < dst->nlimbs; i++) {
        dst->limbs[i] = 0;
    }
    dst->sign = src->sign;
}

int bn_cmp(const bignum *a, const bignum *b) {
    (void)a;
    (void)b;
    /* TODO: Phase 1 */
    return 0;
}

void bn_add(bignum *r, const bignum *a, const bignum *b) {
    (void)r; (void)a; (void)b;
    /* TODO: Phase 1 */
}

void bn_sub(bignum *r, const bignum *a, const bignum *b) {
    (void)r; (void)a; (void)b;
    /* TODO: Phase 1 */
}

void bn_mul(bignum *r, const bignum *a, const bignum *b) {
    (void)r; (void)a; (void)b;
    /* TODO: Phase 1 */
}

void bn_divmod(bignum *q, bignum *rem, const bignum *a, const bignum *b) {
    (void)q; (void)rem; (void)a; (void)b;
    /* TODO: Phase 1 */
}

void bn_shl(bignum *r, const bignum *a, size_t bits) {
    (void)r; (void)a; (void)bits;
    /* TODO: Phase 1 */
}

void bn_shr(bignum *r, const bignum *a, size_t bits) {
    (void)r; (void)a; (void)bits;
    /* TODO: Phase 1 */
}

int bn_from_hex(bignum *r, const char *hex) {
    (void)r; (void)hex;
    /* TODO: Phase 1 */
    return -1;
}

int bn_to_hex(const bignum *a, char *buf, size_t buflen) {
    (void)a; (void)buf; (void)buflen;
    /* TODO: Phase 1 */
    return -1;
}

int bn_from_dec(bignum *r, const char *dec) {
    (void)r; (void)dec;
    /* TODO: Phase 1 */
    return -1;
}

int bn_to_dec(const bignum *a, char *buf, size_t buflen) {
    (void)a; (void)buf; (void)buflen;
    /* TODO: Phase 1 */
    return -1;
}
