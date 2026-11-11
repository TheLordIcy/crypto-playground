#ifndef PRIMES_H
#define PRIMES_H

#include "bignum.h"

int  is_prime(const bignum *n);            /* Miller-Rabin, deterministic < 2^64 */
void gen_prime(bignum *r, size_t bits);     /* random prime of given bit length */

#endif /* PRIMES_H */
