#pragma once

#include "impl.hpp"

namespace rng
{
	struct xoroshiro128plus : public RNG
	{
		using State = std::array<std::uint64_t, 2>;
		inline constexpr xoroshiro128plus(std::uint64_t s0, std::uint64_t s1) : state({s0, s1}){}
		inline constexpr xoroshiro128plus(std::array<std::uint64_t, 2>&& ar) : state(ar){}
		inline constexpr xoroshiro128plus(const std::array<std::uint64_t, 2>& ar) : state(ar){}
	       	inline constexpr xoroshiro128plus(const std::uint64_t (&ar)[2]) : state({ar[0], ar[1]}){} 

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
}
