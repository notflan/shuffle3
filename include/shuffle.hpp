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

	namespace {
		extern "C" int _can_allocate(std::size_t bytes);
		template<typename T>
		inline bool can_allocate(std::size_t len)
		{
			return !!_can_allocate(len*sizeof(T));
		}
	}

	template<typename T, typename R>
	inline void unshuffle(R& rng, span<T> span)
	{
		if(!span.size()) return;

#define DYN 2
#define MAP 3
#if defined(_FS_SPILL_BUFFER) && (_FS_SPILL_BUFFER == DYN)
		D_dprintf("spill=dyn");
		dynamic_spill_vector<std::size_t> rng_values = 
		//can_allocate<std::size_t>(span.size()) //Is there any way we can not waste this malloc() when it's valid?
		span.size() <= FSV_DEFAULT_SPILL_AT
			? dynamic_spill_vector<std::size_t> (span.size(), FSV_DEFAULT_SPILL_AT)
			: dynamic_spill_vector<std::size_t> (FSV_DEFAULT_SPILL_AT);
#elif defined(_FS_SPILL_BUFFER) && (_FS_SPILL_BUFFER == MAP)
		D_dprintf("spill=map");
		mapped_spill_vector<std::size_t> rng_values =
		span.size() <= FSV_DEFAULT_SPILL_AT
			? mapped_spill_vector<std::size_t> (span.size(), FSV_DEFAULT_SPILL_AT)
			: mapped_spill_vector<std::size_t> (FSV_DEFAULT_SPILL_AT, mapped_vector<std::size_t>::from_temp(span.size() - FSV_DEFAULT_SPILL_AT));
#elif defined(_FS_SPILL_BUFFER)
		D_dprintf("spill=static");
		fixed_spill_vector<std::size_t> rng_values;
#else
		D_dprintf("spill=none");
		std::vector<std::size_t> rng_values;
		rng_values.reserve(span.size());
#endif
#undef MAP
#undef DYN

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
