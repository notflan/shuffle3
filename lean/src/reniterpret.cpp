#include <shuffle3.h>
#include <reinterpret.hpp>

template<typename T>
static inline T* bytes_to_t(std::uint8_t* ptr, std::size_t ptr_sz, std::size_t* restrict nsize)
{
	span<uint8_t> bytes(ptr, ptr_sz);
	auto tout = bytes.reinterpret<T>();
	*nsize = tout.size();
	return tout.as_ptr();
}

extern "C" {	
	uint64_t* bytes_to_long(uint8_t* ptr, size_t ptr_sz, size_t* restrict nsize)
	{
		return bytes_to_t<uint64_t>(ptr, ptr_sz, nsize);
	}
	float*    bytes_to_float(uint8_t* ptr, size_t ptr_sz, size_t* restrict nsize)
	{
		return bytes_to_t<float>(ptr, ptr_sz, nsize);
	}
}
