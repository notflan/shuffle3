#include <rng_impl.h>
#include <stdint.h>

/*  Written in 2016-2018 by David Blackman and Sebastiano Vigna (vigna@acm.org)

To the extent possible under law, the author has dedicated all copyright
and related and neighboring rights to this software to the public domain
worldwide. This software is distributed without any warranty.

See <http://creativecommons.org/publicdomain/zero/1.0/>. */

static inline uint64_t rotl(const uint64_t x, int k) {
 return (x << k) | (x >> (64 - k));
}

_Atomic static uint64_t s[2];

static uint64_t next(void) {
	const uint64_t s0 = s[0];
	uint64_t s1 = s[1];
	const uint64_t result = s0 + s1;

	s1 ^= s0;
	s[0] = rotl(s0, 24) ^ s1 ^ (s1 << 16); // a, b
	s[1] = rotl(s1, 37); // c

	return result;
}


static void jump(void) {
	static const uint64_t JUMP[] = { 0xdf900294d8f554a5, 0x170865df4b3201fc };

	uint64_t s0 = 0;
	uint64_t s1 = 0;
	for(int i = 0; i < sizeof JUMP / sizeof *JUMP; i++)
		for(int b = 0; b < 64; b++) {
			if (JUMP[i] & UINT64_C(1) << b) {
				s0 ^= s[0];
    				s1 ^= s[1];
			}
			next();
  		}

	s[0] = s0;
	s[1] = s1;
}

static void long_jump(void) {
	static const uint64_t LONG_JUMP[] = { 0xd2a98b26625eee7b, 0xdddf9b1090aa7ac1 };

	uint64_t s0 = 0;
	uint64_t s1 = 0;
	for(int i = 0; i < sizeof LONG_JUMP / sizeof *LONG_JUMP; i++)
		for(int b = 0; b < 64; b++) {
			if (LONG_JUMP[i] & UINT64_C(1) << b) {
				s0 ^= s[0];
				s1 ^= s[1];
			}
			next();
		}

	s[0] = s0;
	s[1] = s1;
}

static void seed(uint64_t s0, uint64_t s1) {
	s[0] = s0;
	s[1] = s1;
}

void _seed(const void* from)
{
	const uint64_t *ptr = from;
	seed(ptr[0], ptr[1]);
}

double _sample() 
{
	return (next() & ((INT64_C(1) << 53) - 1)) * (1.00 / (INT64_C(1) << 53));
}

RNG_IMPL_DEFINITION(xoroshiro128plus)
{
	struct rng_algo algo;
	algo.sample = &_sample;
	algo.seed = &_seed;

	algo.construct=NULL;
	algo.destruct=NULL;

	return rng_new_impl(&algo); 
}

void xoro_jump()
{
	jump();
}

void xoro_long_jump()
{
	long_jump();
}
