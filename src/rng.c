#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdint.h>
#include <rng.h>

#include <rng_impl.h>

RNG rng_new_impl(const struct rng_algo *from)
{
	RNG algo = malloc(sizeof(struct rng_algo));
	memcpy(algo, from, sizeof(struct rng_algo));
	if(algo->construct!=NULL)
		algo->construct();
	return algo;
}

RNG rng_new(RNG (*instantiate)(void))
{
	return instantiate();
}

void rng_free(RNG algo)
{
	if(algo->destruct!=NULL)
		algo->destruct();
	free(algo);
}

void rng_seed(RNG algo, void* seed)
{
	if(algo->seed!=NULL)
		algo->seed(seed);
}

double rng_next_double(RNG algo)
{
	return algo->sample();
}

int rng_next_int_bounded(RNG algo, int min, int max)
{
	return (int)floor(algo->sample() * (double)(max-min))+min;
}

int rng_next_int(RNG algo, int max)
{
	return (int)floor(algo->sample() * (double)max);
}

void* rng_next(RNG algo, void* data, int len)
{
	uint8_t *buffer = data;
	for(register int i=0;i<len;i++)
	{
		buffer[i] = (uint8_t)rng_next_int(algo, 255);
	}
	return data;
}

int rng_chance(RNG algo, double d)
{
	if(d<=0) return 0;
	return algo->sample()<=d;
}
