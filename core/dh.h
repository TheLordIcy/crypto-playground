#ifndef DH_H
#define DH_H

#include "bignum.h"

void dh_keypair(bignum *priv, bignum *pub, const bignum *p, const bignum *g);
void dh_shared(bignum *s, const bignum *priv, const bignum *peer_pub, const bignum *p);

#endif /* DH_H */
