#include <rng/impl.hpp>
#include <rng/frng.hpp>
#include <cmath>

template<std::size_t N>
constexpr inline double dot(const std::array<double, N>& v, const std::array<double, N>& u)
{
	double res=0;
	for(std::size_t i=0;i<N;i++)
	{
		res += v[i] * u[i];
	}
	return res;
}

inline constexpr double fract(double x)
{
	return x - floor(x);
}

namespace rng {
	inline constexpr double sample_double(const std::array<double, 2>& state)
	{
		const constexpr std::array<double, 2> vec2 = {  12.9898, 78.223  };
		return fract(sin(dot(state, vec2)) * 43758.5453);
	}
	inline void update_state(std::array<double, 2>& state, double r)
	{
		float v1 = (float)state[0];
		float v2 = (float)state[1];

		std::array<double, 2> nvec = {
			r,
			(double)v2,
		};

		state[0] = sample_double(nvec);

		nvec[1] = (double)v1;
		state[1] = sample_double(nvec);
	}
	double frng::sample()
	{
		double res = sample_double(state);
		update_state(state, res);
		return res;
	}
}
