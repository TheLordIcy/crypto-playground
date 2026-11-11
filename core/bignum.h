#ifndef BIGNUM_H
#define BIGNUM_H

#include <stdint.h>
#include <stddef.h>

/* Magnitude-only big integer. Limbs are uint32_t; arithmetic uses a uint64_t
 * accumulator so carry propagation cannot overflow. See Roadmap.md Phase 1. */
typedef struct {
    uint32_t *limbs;
    size_t     nlimbs;
    int        sign;   /* +1 or -1; 0 means zero */
} bignum;

/* Lifecycle */
bignum *bn_new(size_t nlimbs);
void    bn_free(bignum *a);

/* Assignment / comparison */
void bn_copy(bignum *dst, const bignum *src);
int  bn_cmp(const bignum *a, const bignum *b);   /* -1 / 0 / +1 */

/* Arithmetic (result may alias inputs where noted) */
void bn_add(bignum *r, const bignum *a, const bignum *b);
void bn_sub(bignum *r, const bignum *a, const bignum *b);
void bn_mul(bignum *r, const bignum *a, const bignum *b);   /* schoolbook */
void bn_divmod(bignum *q, bignum *rem, const bignum *a, const bignum *b);
void bn_shl(bignum *r, const bignum *a, size_t bits);
void bn_shr(bignum *r, const bignum *a, size_t bits);

/* Conversion */
int  bn_from_hex(bignum *r, const char *hex);
int  bn_to_hex(const bignum *a, char *buf, size_t buflen);
int  bn_from_dec(bignum *r, const char *dec);
int  bn_to_dec(const bignum *a, char *buf, size_t buflen);

#endif /* BIGNUM_H */
