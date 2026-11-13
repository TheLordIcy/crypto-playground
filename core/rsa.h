#ifndef RSA_H
#define RSA_H

#include "bignum.h"

typedef struct {
    Bignum *n, *e, *d;
    Bignum *p, *q;
} rsa_keypair;

rsa_keypair *rsa_keygen(size_t bits);
void         rsa_free(rsa_keypair *kp);
void         rsa_encrypt(Bignum *c, const Bignum *m, const rsa_keypair *kp);
void         rsa_decrypt(Bignum *m, const Bignum *c, const rsa_keypair *kp);

#endif /* RSA_H */
