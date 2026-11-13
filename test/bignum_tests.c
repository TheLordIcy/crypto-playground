/* test/bignum_tests.c — unit tests for the Phase 1 bignum lifecycle,
 * capacity, assignment, normalisation and bit-query API.
 *
 * Build/run:  mingw32-make bignum-test
 * (also runs as part of `mingw32-make test`) */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "bignum.h"

static int passed = 0;
static int failed = 0;

#define CHECK(cond) do { \
    if (cond) { passed++; } \
    else { failed++; fprintf(stderr, "FAIL: %s (%s:%d)\n", #cond, __FILE__, __LINE__); } \
} while (0)

static void test_init_free(void)
{
    Bignum b;
    bn_init(&b);
    CHECK(b.limbs == NULL);
    CHECK(b.len == 0);
    CHECK(b.cap == 0);

    CHECK(bn_is_zero(&b) == 1);
    CHECK(bn_bit_length(&b) == 0);

    bn_free(&b);
    CHECK(b.limbs == NULL);
    CHECK(b.len == 0);
    CHECK(b.cap == 0);
}

static void test_set_uint(void)
{
    Bignum b;
    bn_init(&b);

    CHECK(bn_set_uint(&b, 0) == BN_OK);
    CHECK(bn_is_zero(&b) == 1);
    CHECK(b.len == 0);                     /* normalised: no limbs in use */
    CHECK(bn_bit_length(&b) == 0);

    CHECK(bn_set_uint(&b, 1) == BN_OK);
    CHECK(b.len == 1);
    CHECK(b.limbs[0] == 1u);
    CHECK(bn_bit_length(&b) == 1);
    CHECK(bn_is_zero(&b) == 0);

    CHECK(bn_set_uint(&b, 0xFFFFFFFFu) == BN_OK);
    CHECK(b.len == 1);
    CHECK(b.limbs[0] == 0xFFFFFFFFu);
    CHECK(bn_bit_length(&b) == 32);

    /* First value that needs a second limb. */
    CHECK(bn_set_uint(&b, 0x100000000ull) == BN_OK);
    CHECK(b.len == 2);
    CHECK(b.limbs[0] == 0u);
    CHECK(b.limbs[1] == 1u);
    CHECK(bn_bit_length(&b) == 33);

    CHECK(bn_set_uint(&b, UINT64_MAX) == BN_OK);
    CHECK(b.len == 2);
    CHECK(b.limbs[0] == 0xFFFFFFFFu);
    CHECK(b.limbs[1] == 0xFFFFFFFFu);
    CHECK(bn_bit_length(&b) == 64);

    bn_free(&b);
}

static void test_reserve_grows(void)
{
    Bignum b;
    bn_init(&b);

    CHECK(bn_reserve(&b, 0) == BN_OK);
    CHECK(b.cap == 0);

    CHECK(bn_reserve(&b, 10) == BN_OK);
    CHECK(b.cap >= 10);
    CHECK(b.limbs != NULL);

    /* Reserving less than the current capacity must not shrink it. */
    {
        size_t cap = b.cap;
        CHECK(bn_reserve(&b, 2) == BN_OK);
        CHECK(b.cap == cap);
    }

    bn_free(&b);
}

static void test_copy(void)
{
    Bignum a, b;
    bn_init(&a);
    bn_init(&b);

    CHECK(bn_set_uint(&a, 0x123456789ABCDEF0ull) == BN_OK);
    CHECK(bn_copy(&b, &a) == BN_OK);
    CHECK(b.len == a.len);
    CHECK(bn_is_zero(&a) == bn_is_zero(&b));
    CHECK(bn_bit_length(&b) == bn_bit_length(&a));
    CHECK(b.limbs[0] == a.limbs[0]);
    CHECK(b.limbs[1] == a.limbs[1]);

    /* Deep copy: mutating one must not affect the other. */
    b.limbs[0] = 0;
    CHECK(a.limbs[0] == 0x9ABCDEF0u);

    /* Self-copy is a no-op and must not corrupt the value. */
    CHECK(bn_copy(&a, &a) == BN_OK);
    CHECK(a.len == 2);

    bn_free(&a);
    bn_free(&b);
}

static void test_zero(void)
{
    Bignum b;
    bn_init(&b);
    CHECK(bn_set_uint(&b, 42) == BN_OK);
    bn_zero(&b);
    CHECK(b.len == 0);
    CHECK(bn_is_zero(&b) == 1);

    /* Value can be reused after zeroing. */
    CHECK(bn_set_uint(&b, 7) == BN_OK);
    CHECK(b.len == 1);
    CHECK(b.limbs[0] == 7u);

    bn_free(&b);
}

static void test_normalize(void)
{
    Bignum b;
    uint32_t limbs[4] = { 5u, 0u, 0u, 0u };
    bn_init(&b);

    /* Manually build an un-normalised value: 5 with len 4. */
    b.limbs = limbs;
    b.len = 4;
    b.cap = 4;
    bn_normalize(&b);
    CHECK(b.len == 1);
    CHECK(b.limbs[0] == 5u);

    bn_free(&b);   /* limbs points at the stack array: test-only misuse */
}

static void test_get_bit(void)
{
    Bignum b;
    bn_init(&b);

    /* 10 = 0b1010 */
    CHECK(bn_set_uint(&b, 10) == BN_OK);
    CHECK(bn_get_bit(&b, 0) == 0);
    CHECK(bn_get_bit(&b, 1) == 1);
    CHECK(bn_get_bit(&b, 2) == 0);
    CHECK(bn_get_bit(&b, 3) == 1);
    CHECK(bn_get_bit(&b, 4) == 0);
    CHECK(bn_get_bit(&b, 1000) == 0);   /* beyond len */

    /* Bit 32 lives in the second limb. */
    CHECK(bn_set_uint(&b, 0x100000000ull) == BN_OK);
    CHECK(bn_get_bit(&b, 32) == 1);
    CHECK(bn_get_bit(&b, 31) == 0);

    bn_free(&b);
}

int main(void)
{
    test_init_free();
    test_set_uint();
    test_reserve_grows();
    test_copy();
    test_zero();
    test_normalize();
    test_get_bit();

    printf("bignum tests: %d passed, %d failed\n", passed, failed);
    return failed ? EXIT_FAILURE : EXIT_SUCCESS;
}
