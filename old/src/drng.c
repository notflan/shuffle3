#include <rng_impl.h>
#include <string.h>
#include <stdlib.h>

static void _seed(const void* seed)
{
	srand(*((const unsigned int*)seed));
}

static double _next()
{
	return (double)rand() / (double)RAND_MAX;
}

RNG_IMPL_DEFINITION(drng)
{
	struct rng_algo algo;
	memset(&algo,0,sizeof(algo));

	algo.seed= &_seed;
	algo.sample = &_next;

	return rng_new_impl(&algo);
}
