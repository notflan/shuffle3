#pragma once

#include <rng.h>
#include <reinterpret.h>
#include <algorithm>
#include <iostream>

#include <fsvec.hpp>

#include <shuffle3.h>

namespace rng {
	template<typename T, typename R>
	inline void shuffle(R& rng, span<T> span)
	{
		if(!span.size()) return;
		std::cout << " -> shuffling " << span.size() << " objects...";
		for(std::size_t i=span.size()-1;i>0;i--)
		{
			auto j = rng.next_long(i);
			std::swap(span[i], span[j]);
		}
		std::cout << " OK" << std::endl;
	}

	template<typename T, typename R>
	inline void unshuffle(R& rng, span<T> span)
	{
		if(!span.size()) return;

#ifdef _FS_SPILL_BUFFER
		fixed_spill_vector<std::size_t> rng_values;//(span.size()); //TODO: dynamic_spill_vector
#else
		std::vector<std::size_t> rng_values(span.size());
#endif

		std::cout << " -> unshuffling " << span.size() << " objects...";
		for(std::size_t i=span.size()-1;i>0;i--)
			rng_values.push_back(rng.next_long(i));

		for(std::size_t i=1;i<span.size();i++) {
			std::swap(span[i], span[rng_values.back()]);
			rng_values.pop_back();
		}
		std::cout << " OK" << std::endl;
	}
}
