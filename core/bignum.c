/*
 * core/bignum.c -- arbitrary-precision unsigned integers.
 *
 * Phase 1 of Roadmap.md. This file implements the contract declared in
 * bignum.h; the header is the specification, this is the implementation.
 *
 * Representation (see bignum.h):
 *   limbs are little-endian, 32 bits each: value = sum(limb[i] * 2^(32*i))
 *
 * The "normalized" form is the single source of truth: len == 0 means the
 * value is zero, and a non-zero value never has a zero top limb. Every
 * function here leaves its result normalized, so callers never have to
 * second-guess a length.
 *
 * Educational only. Not constant-time, not for protecting real data.
 */

#include "bignum.h"

#include <stdlib.h>
#include <string.h>

/* ---------------------------------------------------------------- lifecycle */

void bn_init(Bignum *b)
{
    b->limbs = NULL;
    b->len   = 0;
    b->cap   = 0;
}

void bn_free(Bignum *b)
{
    free(b->limbs);
    b->limbs = NULL;
    b->len   = 0;
    b->cap   = 0;
}

void bn_zero(Bignum *b)
{
    /* len == 0 *is* zero. Limbs at indices >= len are unspecified by the
     * contract, so there is nothing to clear. */
    b->len = 0;
}

/* ----------------------------------------------------------------- capacity */

/*
 * Grow the limb array so it can hold at least min_cap limbs.
 *
 * Growth doubles the current capacity so that repeated small reservations
 * (say, one limb per addition in a loop) cost amortized O(1) instead of
 * reallocating on every call. This is the one place we are allowed to
 * allocate: the roadmap's "no allocation inside arithmetic functions" rule
 * refers to add/mul/divmod, not to preparing a buffer.
 */
int bn_reserve(Bignum *b, size_t min_cap)
{
    uint32_t *grown;
    size_t    new_cap;

    if (min_cap <= b->cap)
        return BN_OK;

    new_cap = (b->cap > 0) ? b->cap : 1;
    while (new_cap < min_cap) {
        if (new_cap > SIZE_MAX / 2)
            break;                  /* doubling would overflow size_t */
        new_cap *= 2;
    }
    if (new_cap < min_cap)
        new_cap = min_cap;          /* the overflow case: take the request */

    if (new_cap > SIZE_MAX / sizeof *grown)
        return BN_ENOMEM;           /* new_cap * sizeof would overflow */

    grown = (uint32_t *)realloc(b->limbs, new_cap * sizeof *grown);
    if (grown == NULL)
        return BN_ENOMEM;           /* old buffer is still valid */

    b->limbs = grown;
    b->cap   = new_cap;
    return BN_OK;
}

/* --------------------------------------------------------------- assignment */

int bn_copy(Bignum *dst, const Bignum *src)
{
    if (dst == src)
        return BN_OK;               /* self-copy: nothing to do */

    if (src->len == 0) {
        bn_zero(dst);
        return BN_OK;
    }

    if (bn_reserve(dst, src->len) != BN_OK)
        return BN_ENOMEM;

    memcpy(dst->limbs, src->limbs, src->len * sizeof *dst->limbs);
    dst->len = src->len;
    return BN_OK;                   /* src is normalized, so dst is too */
}

/*
 * Set b = v for a 64-bit value. Two limbs hold any uint64_t, so this needs
 * no loop. Note we only ask for the second limb when the value really needs
 * it -- reserving 2 limbs unconditionally would double memory for every
 * small constant we ever store.
 */
int bn_set_uint(Bignum *b, uint64_t v)
{
    b->len = 0;

    if (v == 0)
        return BN_OK;               /* already zero */

    if (bn_reserve(b, 1) != BN_OK)
        return BN_ENOMEM;

    b->limbs[0] = (uint32_t)(v & 0xFFFFFFFFu);
    b->len      = 1;

    if (v > 0xFFFFFFFFu) {
        if (bn_reserve(b, 2) != BN_OK) {
            b->len = 0;             /* keep the invariant: do not half-set */
            return BN_ENOMEM;
        }
        b->limbs[1] = (uint32_t)(v >> 32);
        b->len      = 2;
    }

    return BN_OK;
}

/* ------------------------------------------------- normalisation and queries */

void bn_normalize(Bignum *b)
{
    while (b->len > 0 && b->limbs[b->len - 1] == 0)
        b->len--;
}

int bn_is_zero(const Bignum *b)
{
    /* True only because every producer leaves the value normalized. */
    return b->len == 0;
}

size_t bn_bit_length(const Bignum *b)
{
    uint32_t top;
    size_t   bits = 0;

    if (b->len == 0)
        return 0;

    /* Count the significant bits of the last (most significant) limb. */
    top = b->limbs[b->len - 1];
    while (top != 0) {
        bits++;
        top >>= 1;
    }

    return (b->len - 1) * BN_LIMB_BITS + bits;
}

int bn_get_bit(const Bignum *b, size_t i)
{
    size_t limb = i / BN_LIMB_BITS;

    if (limb >= b->len)
        return 0;                   /* above the number: an implicit zero */

    return (int)((b->limbs[limb] >> (i % BN_LIMB_BITS)) & 1u);
}
