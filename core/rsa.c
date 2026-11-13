#include "rsa.h"

#include <stdlib.h>

rsa_keypair *rsa_keygen(size_t bits) {
    (void)bits;
    /* TODO: Phase 3 */
    return NULL;
}

void rsa_free(rsa_keypair *kp) {
    if (kp == NULL) {
        return;
    }
    bn_free(kp->n);
    bn_free(kp->e);
    bn_free(kp->d);
    bn_free(kp->p);
    bn_free(kp->q);
    free(kp);
}

void rsa_encrypt(Bignum *c, const Bignum *m, const rsa_keypair *kp) {
    (void)c; (void)m; (void)kp;
    /* TODO: Phase 3 */
}

void rsa_decrypt(Bignum *m, const Bignum *c, const rsa_keypair *kp) {
    (void)m; (void)c; (void)kp;
    /* TODO: Phase 3 */
}
