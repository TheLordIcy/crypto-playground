#include "dh.h"

void dh_keypair(Bignum *priv, Bignum *pub, const Bignum *p, const Bignum *g) {
    (void)priv; (void)pub; (void)p; (void)g;
    /* TODO: Phase 4 */
}

void dh_shared(Bignum *s, const Bignum *priv, const Bignum *peer_pub, const Bignum *p) {
    (void)s; (void)priv; (void)peer_pub; (void)p;
    /* TODO: Phase 4 */
}
