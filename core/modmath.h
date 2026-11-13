#ifndef MODMATH_H
#define MODMATH_H

#include "bignum.h"

void modpow(Bignum *r, const Bignum *base, const Bignum *exp, const Bignum *mod);
void modinv(Bignum *r, const Bignum *a, const Bignum *mod);

#endif /* MODMATH_H */
