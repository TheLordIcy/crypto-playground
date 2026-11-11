#ifndef MODMATH_H
#define MODMATH_H

#include "bignum.h"

void modpow(bignum *r, const bignum *base, const bignum *exp, const bignum *mod);
void modinv(bignum *r, const bignum *a, const bignum *mod);

#endif /* MODMATH_H */
