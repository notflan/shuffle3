#include <stdexcept>

#include <iostream>
#include <cmath>
#include <sys/types.h>

#include <rng/impl.hpp>

#include <error.h>
#include <panic.h>


//#define IMPORT_ONE(NS, NAME) using NAME = NS :: NAME

//IMPORT_ONE(std, ssize_t);

template<typename To, typename T, typename Unit=unsigned char>
static inline To* offset_ptr(T* ptr, ssize_t by) noexcept
{
	return reinterpret_cast<To*>(reinterpret_cast<Unit*>(ptr)+by);
}
/*
template<typename To, typename Unit>
offset_ptr(auto* p, std::ssize_t by) -> offset_ptr<To, decltype(p), Unit>;
template<typename To>
offset_ptr(auto* p, std::ssize_t by) -> offset_ptr<To, decltype(p)>;
*/
#ifdef FT_PT_OPERATORS
inline  operator RNG*(rng_t ptr) noexcept
{
	if(__builtin_expect(!ptr, false)) return nullptr;
#ifdef DEBUG
	RNG* op =
#else
	return
#endif
	reinterpret_cast<RNG*>(offset_ptr<RNG>(ptr, -static_cast<ssize_t>(offsetof(RNG, _held))));
#ifdef DEBUG
	if(__builtin_expect(!op, false)) return nullptr;
	if(__builtin_expect(op->_held, false)) panic("Invalid rng_t -> RNG conversion");
	return ptr;
#endif
}

RNG::operator rng_t() const noexcept
{
	return reinterpret_cast<rng_t>(static_cast<RNG*>(this)->_held);
}
#endif

inline unsigned char RNG::byte()
{
	return (unsigned char)next_int(255);
}

void RNG::bytes(unsigned char* ptr, std::size_t len)
{
	for(std::size_t i=0;i<len;i++)
	{
		ptr[i] = byte();
	}
}

bool RNG::chance()
{
	return chance(.5);
}

bool RNG::chance(double d)
{
	if (d<=0) return false;
	return sample() <= d;
}

std::int32_t RNG::next_int(std::int32_t min, std::int32_t max)
{
	return (std::int32_t)floor(sample() * (double)(max-min))+min;
}

std::int32_t RNG::next_int()
{
	return (chance() ? 1 : -1) * (std::int32_t)floor(sample() * (double)INT32_MAX);
}

std::int64_t RNG::next_long(std::int64_t min, std::int64_t max)
{
	return (std::int64_t)floor(sample() * (double)(max-min))+min;
}

std::int64_t RNG::next_long()
{
	return (chance() ? 1l : -1l) * (std::int64_t)floor(sample() * (double)INT64_MAX);
}

#include <rng.h>
namespace { // C interface
	using namespace std;
	[[gnu::always_inline, gnu::gnu_inline]]
	static inline RNG* extract_ptr(rng_t ptr)
	{ return reinterpret_cast<RNG*> (ptr); }
	static inline RNG& extract_ref(rng_t rng)
	{
		return *extract_ptr(rng);
	}
	template<std::derived_from<RNG> T>
	static inline rng_t wrap_ptr(T* ptr) noexcept
	{
		if(__builtin_expect(!ptr, false)) return nullptr;
		return reinterpret_cast<rng_t>(static_cast<RNG*>(ptr));
	}
	template<typename T>
		static inline T* extract_downcast_ptr(rng_t rng)
		{
			return dynamic_cast<T*>(extract_ptr(rng));
		}

	extern "C" 
	{
		rng_t rng_new(rng_init_opt opt)
		{
			switch(opt.kind)
			{
				case RNG_KIND_FRNG:  return wrap_ptr( new rng::frng(opt.init.frng.state) );
				case RNG_KIND_DRNG:  return wrap_ptr( new rng::drng(opt.init.drng.state) );
				case RNG_KIND_XORNG: return wrap_ptr( new rng::xoroshiro128plus(opt.init.xorng.state) );
				//case RNG_KIND_LORENX: return static_cast<rng_t>( new rng::lorenzAttractor(opt.init.lorenz); );
				default: panic("Unknown RNG init opt: %d", opt.kind);
					__builtin_unreachable();
			}
			return nullptr;
		}

		void rng_free(rng_t rng)
		{
			RNG* ptr = extract_ptr(rng);
			delete ptr;
		}

		[[gnu::nonnull(1)]]
		void rng_test_spec(rng_t rng)
		{
			auto& ref = extract_ref(rng);
			cout << "rng_test_spec (" << typeid(ref).name() << ")..." << std::flush;
			if constexpr(!is_noexcept) { 
#if EXCEPT
				try {
					rng::test_algo(std::move(ref));
				}
				catch(const std::exception& except) {
					cout << "\r\r\r\tERROR: " << (except.what() ?: typeid(except).name()) << endl;
					rng_free(rng);
					throw;
				} catch(...) {
					cout << "\r\r\r\tERROR" << endl;
					rng_free(rng);
					throw;
				}
#endif
			} else	rng::test_algo(std::move(ref));
			
			cout << "\r\r\r\tOK" << endl;
			rng_free(rng);
		}
	}
}
