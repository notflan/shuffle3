#ifndef _REINTERPRET_H
#define _REINTERPRET_H

#include <panic.h>

#ifdef __cplusplus
#define restrict __restrict__
#include <cstddef>
#include <cstdint>
#else
#include <stddef.h>
#include <stdint.h>
#endif

#ifdef __cplusplus
template<typename T>
struct span {
	inline span(T* ptr, std::size_t len) : ptr(ptr), len(len) {}
	
	template<typename U>
	inline span<U> reinterpret() const
	{
		auto bytes = size_bytes();
		//if (len_b % sizeof(U) != 0) panic("Cannot reinterpret T to U due to unmatch sizing constraints.");

		return span<U>((U*)ptr, bytes / sizeof(U));
	}

	inline const T& operator[](std::size_t i) const
	{
		if (i >= len) panic("Out of bounds access");
		return ptr[i];
	}
	inline T& operator[](std::size_t i)
	{
		if(i >= len) panic("Out of bounds access");
		return ptr[i];
	}

	inline const T* as_ptr() const { return ptr; }
	inline T* as_ptr() { return ptr; }

	inline const T* operator&() const { return as_ptr(); }
	inline T* operator&() { return as_ptr(); }

	inline std::size_t size_bytes() const { return len * sizeof(T); }
	inline std::size_t size() const { return len; }
private:
	T* const ptr;
	const std::size_t len;
};

extern "C" {
#endif
uint64_t* bytes_to_long(uint8_t* ptr, size_t ptr_sz, size_t* restrict nsize);
float*    bytes_to_float(uint8_t* ptr, size_t ptr_sz, size_t* restrict nsize);
#ifdef __cplusplus
}
#endif

#endif /* _REINTERPRET_H */
