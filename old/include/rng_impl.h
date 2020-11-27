#ifndef _RNG_IMPL_H
#define _RNG_IMPL_H

#include "rng.h"
#include <stdlib.h>

struct rng_algo {
	void (*seed)(const void* state);
	double (*sample)(void);

	void (*construct)(void);
	void (*destruct)(void);
}; //*RNG

RNG rng_new_impl(const struct rng_algo *from);

#endif /* _RNG_IMPL_H */
