#ifndef PRIMES_H
#define PRIMES_H

#include "bignum.h"

int  is_prime(const Bignum *n);            /* Miller-Rabin, deterministic < 2^64 */
void gen_prime(Bignum *r, size_t bits);     /* random prime of given bit length */

#endif /* PRIMES_H */
