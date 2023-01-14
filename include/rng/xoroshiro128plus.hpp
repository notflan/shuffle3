#pragma once

#include "impl.hpp"
#include <debug.h>

namespace rng
{
	struct xoroshiro128plus : public RNG
	{
		using State = std::array<std::uint64_t, 2>;
#define P D_dprintf("xorng: seeded with (%lu, %lu)", state[0], state[1]);
		inline constexpr xoroshiro128plus(std::uint64_t s0, std::uint64_t s1) : RNG(), state({s0, s1}){P}
		inline constexpr xoroshiro128plus(std::array<std::uint64_t, 2>&& ar) : RNG(), state(ar){P}
		inline constexpr xoroshiro128plus(const std::array<std::uint64_t, 2>& ar) : RNG(), state(ar){P}
	       	inline constexpr xoroshiro128plus(const std::uint64_t (&ar)[2]) : RNG(), state({ar[0], ar[1]}){P} 
		inline virtual ~xoroshiro128plus() {}
#undef P
		std::uint64_t next_ulong();
		using RNG::next_long;
		std::int64_t  next_long() override;

		void jump();
		void long_jump();
	protected:
		double sample() override;
	private:
		State state;
	};
	static_assert(std::derived_from<xoroshiro128plus, RNG>, "Wtf???");
}

