
#include <rng/impl.hpp>
#include <cmath>

inline unsigned char RNG::byte()
{
	return (unsigned char)next_int(255);
}

void RNG::bytes(unsigned char* ptr, std::size_t len)
{
	for(std::size_t i=0;i<len;i++)
	{
		ptr[i] = byte();
	}
}

bool RNG::chance()
{
	return chance(.5);
}

bool RNG::chance(double d)
{
	if (d<=0) return false;
	return sample() <= d;
}

std::int32_t RNG::next_int(std::int32_t min, std::int32_t max)
{
	return (std::int32_t)floor(sample() * (double)(max-min))+min;
}

std::int32_t RNG::next_int()
{
	return next_int(INT32_MIN, INT32_MAX);
}

std::int64_t RNG::next_long(std::int64_t min, std::int64_t max)
{
	return (std::int64_t)floor(sample() * (double)(max-min))+min;
}

std::int64_t RNG::next_long()
{
	return next_long(INT64_MIN, INT32_MAX);
}
