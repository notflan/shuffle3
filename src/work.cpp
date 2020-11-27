#include <tuple>
#include <functional>
#include <cfloat>

#include <fmt/format.h>

#include <shuffle3.h>
#include <panic.h>
#include <map.h>
#include <reinterpret.h>
#include <shuffle.hpp>

#include <work.h>
#include <debug.h>

template<typename T, typename Fn>
std::tuple<T, T> minmax_t(const span<T>& array, Fn keep)
{
	T highest;
	T lowest;
	bool init=false;
	D_dprintf("minmax_t: %p (%lu)", array.as_ptr(), array.size());
	for(std::size_t i=0;i<array.size();i++)
	{
		if(!keep(array[i])) continue;

		auto item = array[i];
		if(!init) {
			init = true;
			lowest = highest = item;
		}
		else if(item < lowest) lowest = item;
		else if(item > highest) highest = item;
	}
	//fmt::print("MMX {}, {}\n", lowest, highest);
	return {lowest, highest};
}

template<typename T>
inline std::tuple<T, T> minmax_t(const span<T>& array)
{
	return minmax_t(array, [](T _val) { return true; });
}

namespace work
{
	/// Shuffle or unshuffle in place
	template<bool unshuffle>
	int xshuffle_ip(const char* file)
	{
		mm::mmap map(file);

		if constexpr(unshuffle)
		{
			auto [byte_l, byte_h] = minmax_t(map.as_span().reinterpret<std::int8_t>());
			D_dprintf("MMX res (s8): %d -- %d", byte_l, byte_h);
			rng::drng		drng((std::int32_t) ((0xfffa << 16) | (byte_l<<7) | byte_h ));
			rng::unshuffle(drng,  map.as_span());

			auto [float_l, float_h] = minmax_t(map.as_span().reinterpret<float>(), [](float f) -> bool { return !( (f!=f) || f < -FLT_MAX || f > FLT_MAX); });
			D_dprintf("MMX res (f32): %f -- %f", float_l, float_h);
			rng::frng		frng(float_l, float_h);
			rng::unshuffle(frng,  map.as_span().reinterpret<float>());

			auto [long_l, long_h] = minmax_t(map.as_span().reinterpret<std::int64_t>());
			D_dprintf("MMX res (u64): %ld -- %ld", long_l, long_h);
			rng::xoroshiro128plus	xorng(*(const std::uint64_t*)&long_l, *(const std::uint64_t*)&long_h);
			rng::unshuffle(xorng, map.as_span().reinterpret<std::int64_t>());
		} else {
			auto [long_l, long_h] = minmax_t(map.as_span().reinterpret<std::int64_t>());
			D_dprintf("MMX res (u64): %ld -- %ld", long_l, long_h);
			rng::xoroshiro128plus	xorng(*(const std::uint64_t*)&long_l, *(const std::uint64_t*)&long_h);
			rng::shuffle(xorng, map.as_span().reinterpret<std::int64_t>());

			auto [float_l, float_h] = minmax_t(map.as_span().reinterpret<float>(), [](float f) -> bool { return !( (f!=f) || f < -FLT_MAX || f > FLT_MAX); });
			D_dprintf("MMX res (f32): %f -- %f", float_l, float_h);
			rng::frng		frng(float_l, float_h);
			rng::shuffle(frng,  map.as_span().reinterpret<float>());
			
			auto [byte_l, byte_h] = minmax_t(map.as_span().reinterpret<std::int8_t>());
			D_dprintf("MMX res (s8): %d -- %d", byte_l, byte_h);
			rng::drng		drng((std::int32_t) ((0xfffa << 16) | (byte_l<<7) | byte_h ));
			rng::shuffle(drng,  map.as_span());
		}

		return 0;
	}

	/// Shuffle or unshuffle out of place
	template<bool unshuffle>
	int xshuffle_op(const char* ifile, const char* ofile, bool is_buffered)
	{

		if constexpr(unshuffle)
		{

		} else {

		}
		panic("Unimplemented");
		return 0;
	}

}

int help()
{
	//Print help then exit	
	return 1;
}

extern "C" int do_work(const work_args_t args)
{
	using A = decltype(args.op);
	switch (args.op) {
		case A::OP_SHUFFLE_IP: return work::xshuffle_ip<false >(args.data.op_shuffle_ip.file);
		case A::OP_SHUFFLE_OP: return work::xshuffle_op<false >(args.data.op_shuffle_op.ifile,
						    	args.data.op_shuffle_op.ofile,
							args.data.op_shuffle_op.buffered == WORK_BO_BUFFERED);
		case A::OP_UNSHUFFLE_IP: return work::xshuffle_ip<true >(args.data.op_unshuffle_ip.file);
		case A::OP_UNSHUFFLE_OP: return work::xshuffle_op<true>(args.data.op_unshuffle_op.ifile,
						    	args.data.op_unshuffle_op.ofile,
							args.data.op_unshuffle_op.buffered == WORK_BO_BUFFERED);
		case A::OP_HELP: return help();

		default: panic("Unknown op %d", (int)args.op);
	}
	return 0;
}
