#ifndef _RNG_ALGOS_H
#define _RNG_ALGOS_H

#include "rng.h"

RNG_IMPL_DEFINITION(xoroshiro128plus);
void xoro_jump();
void xoro_long_jump();

RNG_IMPL_DEFINITION(frng);

RNG_IMPL_DEFINITION(drng);

#endif /* _RNG_ALGOS_H */
