#include "dh.h"

void dh_keypair(bignum *priv, bignum *pub, const bignum *p, const bignum *g) {
    (void)priv; (void)pub; (void)p; (void)g;
    /* TODO: Phase 4 */
}

void dh_shared(bignum *s, const bignum *priv, const bignum *peer_pub, const bignum *p) {
    (void)s; (void)priv; (void)peer_pub; (void)p;
    /* TODO: Phase 4 */
}
