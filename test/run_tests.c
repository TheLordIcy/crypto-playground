/* test/run_tests.c — tiny assert-based harness.
 * Reports pass/fail counts. Phase 0 done-criterion: a deliberately broken
 * assertion makes the build fail. */
#include <stdio.h>
#include <stdlib.h>

static int passed = 0;
static int failed = 0;

#define CHECK(cond) do { \
    if (cond) { passed++; } \
    else { failed++; fprintf(stderr, "FAIL: %s (%s:%d)\n", #cond, __FILE__, __LINE__); } \
} while (0)

int main(void) {
    /* Phase 0 sanity: a deliberately broken assertion should fail the build/run. */
    CHECK(1 == 1);
    /* Uncomment the next line to verify the harness reports failures: */
    /* CHECK(0 == 1); */

    printf("tests: %d passed, %d failed\n", passed, failed);
    return failed ? EXIT_FAILURE : EXIT_SUCCESS;
}
