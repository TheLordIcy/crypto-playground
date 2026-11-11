#include "sha256.h"

void sha256_init(sha256_ctx *ctx) {
    (void)ctx;
    /* TODO: Phase 4 */
}

void sha256_update(sha256_ctx *ctx, const uint8_t *data, size_t len) {
    (void)ctx; (void)data; (void)len;
    /* TODO: Phase 4 */
}

void sha256_final(sha256_ctx *ctx, uint8_t out[32]) {
    (void)ctx;
    for (size_t i = 0; i < 32; i++) {
        out[i] = 0;
    }
    /* TODO: Phase 4 */
}
