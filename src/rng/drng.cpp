#include <cstdlib>
#include <ctime>
#include <iostream>

#include <rng/drng.hpp>

namespace rng
{
	drng drng::from_time() { return drng(time(NULL)); }

	int drng::rand()
	{
		return rand_r(&state);
	}

	double drng::sample()
	{
		int val = rand_r(&state);
		return (double)val / (double)RAND_MAX;
	}
}
