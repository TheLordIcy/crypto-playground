#ifndef DH_H
#define DH_H

#include "bignum.h"

void dh_keypair(Bignum *priv, Bignum *pub, const Bignum *p, const Bignum *g);
void dh_shared(Bignum *s, const Bignum *priv, const Bignum *peer_pub, const Bignum *p);

#endif /* DH_H */
