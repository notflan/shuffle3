#pragma once

#include <rng.h>
#include <reinterpret.h>
#include <algorithm>
#include <iostream>
#include <chrono>

#include <fmt/format.h>

#include <fsvec.hpp>

#include <perc.h>

#include <shuffle3.h>


namespace rng {
	namespace details [[gnu::visibility("hidden")]] {
		static inline auto make_prog(FILE* to = stdout) noexcept
		{
			if(isatty(fileno(to))) return pr::make_dyn(pr::Progress{to});
			else return pr::make_dyn(pr::disable);
		}

		template<bool Create, bool UseNanos=false> 
		static inline decltype(auto) clock(auto&... last) noexcept {
			using namespace std::chrono;
			if constexpr(Create) return steady_clock::now();
			else {
				constexpr auto take_first = [] (auto& one, auto&...) { return std::forward<decltype(one)>(one); };
				time_point end = steady_clock::now();
				auto& last = take_first(std::forward<decltype(last)>(last)...); //TODO: Expand this....
				auto duration = end - last;
				last = end;
				if constexpr(UseNanos)
					return duration_cast<nanoseconds>(duration).count();
				else	return duration_cast<milliseconds>(duration).count();
			}
		}
	}
	template<typename T, typename R>
	inline void shuffle(R& rng, span<T> span)
	{
		if(!span.size()) return;
		//pr::Progress prog{stdout};
#define MS_OKAY 100
		auto prog = details::make_prog();
		auto timer = clock<true>();
		std::cout << " -> shuffling " << span.size() << " objects..." << std::flush;
		
		prog->spin(0);
		for(std::size_t i=span.size()-1;i>0;i--)
		{
			auto j = rng.next_long(i);
			std::swap(span[i], span[j]);

			if(clock<false>(timer) >= MS_OKAY) {
				prog->aux(fmt::format("{} <-> {}, {} / {}", i, j, span.size() - i, span.size()));
				prog->spin(double(span.size() - i) / double(span.size()));	
			}
		}
		prog->spin(1);
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

		std::cout << " -> unshuffling " << span.size() << " objects..." << std::flush;
		for(std::size_t i=span.size()-1;i>0;i--)
			rng_values.push_back(rng.next_long(i));

		for(std::size_t i=1;i<span.size();i++) {
			std::swap(span[i], span[rng_values.back()]);
			rng_values.pop_back();
		}
		std::cout << " OK" << std::endl;
	}
}
