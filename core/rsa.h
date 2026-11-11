#ifndef RSA_H
#define RSA_H

#include "bignum.h"

typedef struct {
    bignum *n, *e, *d;
    bignum *p, *q;
} rsa_keypair;

rsa_keypair *rsa_keygen(size_t bits);
void         rsa_free(rsa_keypair *kp);
void         rsa_encrypt(bignum *c, const bignum *m, const rsa_keypair *kp);
void         rsa_decrypt(bignum *m, const bignum *c, const rsa_keypair *kp);

#endif /* RSA_H */
