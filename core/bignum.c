#include "bignum.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void bn_init(Bignum *b)
{
    b->limbs = NULL;
    b->len = 0;
    b->cap = 0;
}

void bn_free(Bignum *b)
{
    free(b->limbs);
    bn_init(b);
}

void bn_zero(Bignum *b)
{
    b->len = 0;
}

void bn_normalize(Bignum *b)
{
    while (b->len > 0 && b->limbs[b->len - 1] == 0)
        b->len--;
}

int bn_reserve(Bignum *b, size_t min_cap)
{
    if(min_cap <= b->cap)
        return BN_OK;

    /* Grow Geometrically to keep repeated appends amortized 0(1) */
    size_t new_cap = b->cap ? b ->cap : 4;
    while (new_cap < min_cap) {
        if (new_cap > SIZE_MAX / 2) {
            new_cap = min_cap;
            break;
        }
        new_cap *= 2;
    }
    if (new_cap > SIZE_MAX / sizeof(uint32_t))
        return BN_ENOMEM;

    uint32_t *p = realloc(b->limbs, new_cap * sizeof(uint32_t));
    if (!p)
        return BN_ENOMEM;

    b->limbs = p;
    b->cap = new_cap;
    return BN_OK;
}

int bn_copy(Bignum *dst, const Bignum *src)
{
    if (dst == src)
        return BN_OK;
    if (bn_reserve(dst, src->len) != BN_OK)
        return BN_ENOMEM;
    if (src->len)
        memcpy(dst->limbs, src->limbs, src->len * sizeof(uint32_t));
    dst->len = src->len;
    return BN_OK;
}

int bn_set_uint(Bignum *b, uint64_t v)
{
    if (bn_reserve(b, 2) != BN_OK)
        return BN_ENOMEM;
    b->limbs[0] = (uint32_t)(v & 0xFFFFFFFFu);
    b->limbs[1] = (uint32_t)(v >> 32);
    b->len = 2;
    bn_normalize(b);
    return BN_OK;
}

int bn_is_zero(const Bignum *b)
{
    /* Correct for normalized values. For robustness against un-normalized
     * input, scan instead of trusting len alone. */
    for (size_t i = 0; i < b->len; i++)
        if (b->limbs[i] != 0)
            return 0;
    return 1;
}

size_t bn_bit_length(const Bignum *b)
{
    size_t n = b->len;
    while (n > 0 && b->limbs[n - 1] == 0)   /* tolerate un-normalized input */
        n--;
    if (n == 0)
        return 0;

    uint32_t top = b->limbs[n - 1];
    size_t bits = 0;
    while (top) {
        bits++;
        top >>= 1;
    }
    return (n - 1) * BN_LIMB_BITS + bits;
}

int bn_get_bit(const Bignum *b, size_t i)
{
    size_t limb = i / BN_LIMB_BITS;
    if (limb >= b->len)
        return 0;
    return (int)((b->limbs[limb] >> (i % BN_LIMB_BITS)) & 1u);
}
