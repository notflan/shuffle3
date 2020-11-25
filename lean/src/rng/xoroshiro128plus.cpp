#include <rng/xoroshiro128plus.hpp>
#include <iostream>

using u64 = std::uint64_t;

#define XO xoroshiro128plus

static inline constexpr u64 rotl(u64 x, int k)
{
	return (x << k) | (x >> (64 - k));
}

namespace rng
{
	static inline void xo_test()
	{
		using namespace std;
		XO rng(100ul, 123123123ul);

		for(int i=0;i<10;i++) {
			double d = rng.next_double();
			long l = rng.next_long(-10, 10);

			std::array<bool, 10> ar;
			for(auto& i : ar) i = rng.chance();

			cout << "XO Sampled: " << d << endl; 
			cout << "XO Long: " << l << endl;
			cout << "XO Bools: [ ";
			for(const auto& i : ar) cout << i << " ";
			cout << "]" << endl;
		}
	}

	inline constexpr u64 next(XO::State& s)
	{
		u64 s0 = s[0];
		u64 s1 = s[1];
		u64 result = s0 + s1;

		s1 ^= s0;
		s[0] = rotl(s0, 24) ^ s1 ^ (s1 << 16);
		s[1] = rotl(s1, 37);

		return result;
	}
	inline constexpr void xo_jump(XO::State& s)
	{
		constexpr const std::uint64_t JUMP[] = { 0xdf900294d8f554a5, 0x170865df4b3201fc };

		std::uint64_t s0 = 0;
		std::uint64_t s1 = 0;
		for(u64 i = 0; i < sizeof JUMP / sizeof *JUMP; i++)
			for(int b = 0; b < 64; b++) {
				if (JUMP[i] & UINT64_C(1) << b) {
					s0 ^= s[0];
					s1 ^= s[1];
				}
				next(s);
			}

		s[0] = s0;
		s[1] = s1;
	}

	inline constexpr void xo_long_jump(XO::State& s)
	{
		constexpr const uint64_t LONG_JUMP[] = { 0xd2a98b26625eee7b, 0xdddf9b1090aa7ac1 };

		std::uint64_t s0 = 0;
		std::uint64_t s1 = 0;
		for(u64 i = 0; i < sizeof LONG_JUMP / sizeof *LONG_JUMP; i++)
			for(int b = 0; b < 64; b++) {
				if (LONG_JUMP[i] & UINT64_C(1) << b) {
					s0 ^= s[0];
					s1 ^= s[1];
				}
				next(s);
			}

		s[0] = s0;
		s[1] = s1;
	}
		

	void XO::jump() { xo_jump(state); }
	void XO::long_jump() { xo_long_jump(state); }

	std::uint64_t XO::next_ulong()
	{
		return next(state);
	}
	std:: int64_t XO::next_long()
	{
		const u64 v = next_ulong();
		static_assert(sizeof(v) == sizeof(decltype(next_long())));

		return *(const std::int64_t*)&v;
	}


	double XO::sample()
	{
		return (next(state) & ((INT64_C(1) << 53) - 1)) * (1.00 / (INT64_C(1) << 53));
	}
}

extern "C" void xorng_test()
{
	return rng::xo_test();
}
