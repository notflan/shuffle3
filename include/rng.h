#ifndef _RNG_H
#define _RNG_H

#include "shuffle3.h"

#ifdef __cplusplus
#include "rng/impl.hpp"
extern "C" {
#endif

enum rng_kind {
	RNG_KIND_FRNG,
	RNG_KIND_DRNG,
	RNG_KIND_XORNG,
	RNG_KIND_LORENZ,
};

typedef long double rng_st_lorenz_t;
typedef _Complex long double rng_lorenz_t;

typedef struct rng_init
{
	enum rng_kind kind;
	union {
		struct {
			double state[2];
		} frng;
		struct {
			int32_t state;
		} drng;
		struct {
			uint64_t state[2];
		} xorng;
		struct {
			rng_lorenz_t point;
			const rng_st_lorenz_t (* _UNIQUE state)[5];
			uint64_t iter;
		} lorenz;
	} init;

} rng_init_opt;

typedef struct rng_impl* _UNIQUE rng_t;

rng_t rng_new(rng_init_opt kind);
#define RNG_INIT(_kind,...) ((rng_init_opt){.kind=(_kind), .init.__VA_ARGS__ })
void  rng_free(rng_t ptr);

// Tests
extern void rng_test();
extern void rng_test_spec(rng_t rng) __attribute__((nonnull(1)));

#ifdef __cplusplus
}
// RNG interfaces
#include "rng/xoroshiro128plus.hpp"
#include "rng/frng.hpp"
#include "rng/drng.hpp"
#include "rng/lorenz.hpp"

namespace rng {
	void test_algo(RNG&& rng);

template<class R,
	typename std::enable_if<std::is_base_of<RNG, R>::value>::value __fuck>
		inline void test_algo(R&& rng) { test_algo(static_cast<RNG&&>(rng)); }
}
#endif

#endif /* _RNG_H */
