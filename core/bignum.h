#ifndef BIGNUM_H
#define BIGNUM_H

#include <stddef.h>
#include <stdint.h>

/*
 * Arbitrary-precision unsigned integer
 *
 * Limbs are stored little-endian: limb[0] is the least significant
 * 32 bits. value = sum9limbs[i] * 2^(32*i)) for i in [0,len).
 *
 * Invariants (the "normalized" form):
 *  - len <= cap
 *  - len  == 0
 *  - if len > 0, limbs[len-1] !=0 (no leading zero limbs)
 *
 * any function that can leave leading zero limbs must call bn_normalized()
 * before returning. libms at indices >= len are unspecified
 */
typedef struct {
    uint32_t *limbs;  /* heap array of capacity `cap` (NULL if cap == 0) */
    size_t    len;    /* number of limbs in use */
    size_t    cap;    /* number of limbs allocated */
} Bignum;

#define BN_OK   0
#define BN_ENOMEM (-1)

#define BN_LIMB_BITS 32u

/* Lifecycle */
void bn_init(Bignum *b);
void bn_free(Bignum *b);
void bn_zero(Bignum *b);

/* Capacity */
int bn_reserve(Bignum *b, size_t min_cap);

/* Assignment */
int bn_copy(Bignum *dst, const Bignum *src);
int bn_set_uint(Bignum *b, uint64_t v);

/* Comparison */
/* Returns <0 if a < b, 0 if a == b, >0 if a > b. Both operands must be
 * normalized. Cheap-first: differing lengths decide immediately. */
int bn_cmp(const Bignum *a, const Bignum *b);

/* Normalisation and quaries */
void bn_normalize(Bignum *b);
int  bn_is_zero(const Bignum*b);
size_t bn_bit_length(const Bignum *b);
int    bn_get_bit(const Bignum *b, size_t i);

#endif /* BIGNUM_H */
