/* test/run_tests.c — tiny assert-based harness.
 * Reports pass/fail counts. Phase 0 done-criterion: a deliberately broken
 * assertion makes the build fail. */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "../core/bignum.h"

static int passed = 0;
static int failed = 0;

#define CHECK(cond) do { \
    if (cond) { passed++; } \
    else { failed++; fprintf(stderr, "FAIL: %s (%s:%d)\n", #cond, __FILE__, __LINE__); } \
} while (0)

/* Phase 1 — bignum lifecycle, assignment and queries. */
static void test_bignum_basics(void)
{
    Bignum a, b, t;

    /* Every Bignum must be initialised before use: bn_reserve trusts cap. */
    bn_init(&a);
    bn_init(&b);
    CHECK(a.limbs == NULL);
    CHECK(a.len == 0);
    CHECK(a.cap == 0);
    CHECK(bn_is_zero(&a));
    CHECK(bn_bit_length(&a) == 0);

    /* 0x1234567890ABCDEF needs two limbs: 0x90ABCDEF then 0x12345678. */
    CHECK(bn_set_uint(&a, UINT64_C(0x1234567890ABCDEF)) == BN_OK);
    CHECK(a.len == 2);
    CHECK(a.limbs[0] == UINT32_C(0x90ABCDEF));
    CHECK(a.limbs[1] == UINT32_C(0x12345678));
    CHECK(!bn_is_zero(&a));
    CHECK(bn_bit_length(&a) == 61);

    /* Values that fit in one limb must not waste a second one. */
    CHECK(bn_set_uint(&a, UINT64_C(0xFFFFFFFF)) == BN_OK);
    CHECK(a.len == 1);
    CHECK(a.limbs[0] == UINT32_C(0xFFFFFFFF));
    CHECK(bn_bit_length(&a) == 32);

    CHECK(bn_set_uint(&a, 0) == BN_OK);
    CHECK(a.len == 0);
    CHECK(bn_is_zero(&a));

    /* 2^64 + 1 spans three limbs (limb[i] weighs 2^(32*i)) and has 65 bits. */
    CHECK(bn_reserve(&a, 3) == BN_OK);
    CHECK(a.cap >= 3);
    a.limbs[0] = 1;
    a.limbs[1] = 0;
    a.limbs[2] = 1;
    a.len      = 3;
    CHECK(bn_bit_length(&a) == 65);
    CHECK(bn_get_bit(&a, 0) == 1);
    CHECK(bn_get_bit(&a, 1) == 0);
    CHECK(bn_get_bit(&a, 64) == 1);
    CHECK(bn_get_bit(&a, 65) == 0);   /* beyond the top: implicit zero */
    CHECK(bn_get_bit(&a, 32) == 0);

    /* Normalisation: a stale zero top limb must be trimmed. */
    bn_init(&t);
    CHECK(bn_reserve(&t, 3) == BN_OK);
    t.limbs[0] = 5;
    t.limbs[1] = 0;
    t.limbs[2] = 0;
    t.len      = 3;
    bn_normalize(&t);
    CHECK(t.len == 1);
    CHECK(bn_bit_length(&t) == 3);

    /* Copy is independent: mutating the source must not touch the copy. */
    CHECK(bn_copy(&b, &t) == BN_OK);
    CHECK(b.len == 1);
    CHECK(b.limbs[0] == 5);
    CHECK(bn_copy(&b, &b) == BN_OK);
    CHECK(b.len == 1);
    t.limbs[0] = 77;
    CHECK(b.limbs[0] == 5);

    /* Copying zero clears the destination. */
    CHECK(bn_copy(&b, &a) == BN_OK);
    CHECK(b.len == 3);               /* a is 2^64 + 1 here */
    CHECK(b.limbs[2] == 1);
    CHECK(bn_set_uint(&a, 0) == BN_OK);
    CHECK(bn_copy(&b, &a) == BN_OK);
    CHECK(bn_is_zero(&b));

    /* Growing on demand: many small reservations must not lose a limb. */
    CHECK(bn_reserve(&b, 1) == BN_OK);
    b.limbs[0] = 9;
    b.len      = 1;
    CHECK(bn_reserve(&b, 40) == BN_OK);
    CHECK(b.cap >= 40);
    CHECK(b.limbs[0] == 9);          /* realloc preserved the data */
    CHECK(b.len == 1);

    bn_free(&t);
    bn_free(&b);
    bn_free(&a);
    CHECK(a.limbs == NULL);
    CHECK(a.cap == 0);
}

int main(void) {
    /* Phase 0 sanity: a deliberately broken assertion should fail the build/run. */
    CHECK(1 == 1);
    /* Uncomment the next line to verify the harness reports failures: */
    /* CHECK(0 == 1); */

    test_bignum_basics();

    printf("tests: %d passed, %d failed\n", passed, failed);
    return failed ? EXIT_FAILURE : EXIT_SUCCESS;
}
