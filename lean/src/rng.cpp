#include <iostream>
#include <cmath>

#include <rng/impl.hpp>

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
	return (chance() ? 1 : -1) * (std::int32_t)floor(sample() * (double)INT32_MAX);
}

std::int64_t RNG::next_long(std::int64_t min, std::int64_t max)
{
	return (std::int64_t)floor(sample() * (double)(max-min))+min;
}

std::int64_t RNG::next_long()
{
	return (chance() ? 1l : -1l) * (std::int64_t)floor(sample() * (double)INT64_MAX);
}

#include <rng.h>
#include <panic.h>
namespace { // C interface
	using namespace std;
#define extract_ptr(ptr) ((RNG*)(ptr))
	static inline RNG& extract_ref(rng_t rng)
	{
		return *extract_ptr(rng);
	}
	template<typename T>
		static inline T* extract_downcast_ptr(rng_t rng)
		{
			return dynamic_cast<T*>(extract_ptr(rng));
		}

	extern "C" 
	{

		rng_t rng_new(rng_init_opt opt)
		{
			switch(opt.kind)
			{
				case RNG_KIND_FRNG:  return (rng_t) new rng::frng(opt.init.frng.state);
				case RNG_KIND_DRNG:  return (rng_t) new rng::drng(opt.init.drng.state);
				case RNG_KIND_XORNG: return (rng_t) new rng::xoroshiro128plus(opt.init.xorng.state);
				default: panic("Unknown RNG init opt: %d", opt.kind);
			}
			return nullptr;
		}

		void rng_free(rng_t rng)
		{
			RNG* ptr = (RNG*)rng;
			delete ptr;
		}

		void rng_test_spec(rng_t rng)
		{
			cout << "rng_test_spec:" << endl;
			rng::test_algo(std::move(extract_ref(rng)));	
		}
	}
}
