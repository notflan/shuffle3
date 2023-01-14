#pragma once

#include <array>
#include <vector>
#include <cstdint>

/// Base class for RNG impls
struct RNG {
	constexpr RNG() noexcept = default;

	virtual unsigned char byte();
	virtual void bytes(unsigned char* ptr, std::size_t len);

	template<std::size_t N>
	inline void bytes(unsigned char (&arr)[N]) { return bytes(arr, N); }
	template<std::size_t N>
	inline void bytes(std::array<unsigned char, N>& array) { return bytes(&array[0], N); }
	inline void bytes(std::vector<unsigned char>& vec) { return bytes(&vec[0], vec.size()); }

	bool chance();
	virtual bool chance(double chance);

	virtual std::int32_t next_int();
	inline std::int32_t next_int(std::int32_t max) { return next_int(0, max); }
	std::int32_t next_int(std::int32_t min, std::int32_t max);

	virtual std::int64_t next_long();
       	inline std::int64_t next_long(std::int64_t max) { return next_long(0, max); }
	std::int64_t next_long(std::int64_t min, std::int64_t max);

	inline virtual float  next_float() { return (float)sample(); }
	inline virtual double next_double() { return sample(); }

	constexpr virtual ~RNG() = default;

	//explicit operator rng_t() const noexcept;
	//friend operator RNG*(rng_t) noexcept;
protected:
	virtual double sample() = 0;
//private: 
	//struct rng_impl* _UNIQUE _held = nullptr;
};
