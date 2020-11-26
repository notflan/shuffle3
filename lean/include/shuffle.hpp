#pragma once

#include <rng.h>
#include <reinterpret.h>
#include <algorithm>
#include <fmt/format.h>

namespace rng {
	template<typename T, typename R>
	inline void shuffle(R& rng, span<T> span)
	{
		fmt::print(" -> shuffling {} objects...", span.size());
		for(std::size_t i=span.size()-1;i>0;i--)
		{
			auto j = rng.next_long(i);
			std::swap(span[i], span[j]);
		}
		fmt::print(" OK\n");
	}

	template<typename T, typename R>
	inline void unshuffle(R& rng, span<T> span)
	{
		std::vector<std::size_t> rng_values;

		fmt::print(" -> unshuffling {} objects...", span.size());
		for(std::size_t i=span.size()-1;i>0;i--)
			rng_values.push_back(rng.next_long(i));

		for(std::size_t i=1;i<span.size();i++) {
			std::swap(span[i], span[rng_values.back()]);
			rng_values.pop_back();
		}
		fmt::print(" OK\n");
	}
}
