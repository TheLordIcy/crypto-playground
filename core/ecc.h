#ifndef ECC_H
#define ECC_H

#include "bignum.h"

typedef struct { bignum *x, *y; int infinity; } ecc_point;

void ecc_point_add(ecc_point *r, const ecc_point *a, const ecc_point *b);
void ecc_scalar_mul(ecc_point *r, const bignum *k, const ecc_point *p);

#endif /* ECC_H */
