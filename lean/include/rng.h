#ifndef _RNG_H
#define _RNG_H

#ifdef __cplusplus
extern "C" {
#endif

// Tests
void frng_test();
void xorng_test();
void drng_test();

void rng_test();

#ifdef __cplusplus
}
// RNG interfaces
#include <rng/frng.hpp>
#include <rng/drng.hpp>
#include <rng/xoroshiro128plus.hpp>

namespace rng {
	void test_algo(RNG&& rng);

template<class R,
	typename std::enable_if<std::is_base_of<RNG, R>::value>::value __fuck>
		inline void test_algo(R&& rng) { test_algo(static_cast<RNG&&>(rng)); }
}
#endif

#endif /* _RNG_H */
