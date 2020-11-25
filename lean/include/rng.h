#ifndef _RNG_H
#define _RNG_H

#ifdef __cplusplus
extern "C" {
#endif

// Tests
void frng_test();
void xorng_test();
void drng_test();
#ifdef __cplusplus
}
// RNG interfaces
#include <rng/frng.hpp>
#include <rng/xoroshiro128plus.hpp>

#endif

#endif /* _RNG_H */
