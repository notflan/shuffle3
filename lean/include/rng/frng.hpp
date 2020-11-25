
#include "impl.hpp"
#include <cmath>

namespace rng
{
	struct frng : public RNG 
	{
		template<std::size_t N>
		static constexpr inline double dot(const std::array<double, N>& v, const std::array<double, N>& u)
		{
			double res=0;
			for(std::size_t i=0;i<N;i++)
			{
				res += v[i] * u[i];
			}
			return res;
		}

		static inline constexpr double fract(double x)
		{
			return x - floor(x);
		}
		static inline constexpr double sample_double(const std::array<double, 2>& state)
		{
			const constexpr std::array<double, 2> vec2 = {  12.9898, 78.223  };
			return fract(sin(dot(state, vec2)) * 43758.5453);
		}

		inline frng(double s1, double s2) : state({s1, s2}){}
		inline frng(const std::array<double, 2>& ar) : state(ar){}
		inline frng(std::array<double, 2>&& ar) : state(ar){}
		inline frng(const double (&ar)[2]) : state({ar[0], ar[1]}) {}

		inline constexpr double next_double() override { return sample(); }
		inline constexpr float next_float() override { return (float)sample(); }
	protected: 
		inline constexpr double sample() override
		{
			double res = sample_double(state);
			update_state(state, res);
			return res;
		}
	private:
		std::array<double, 2> state;
		static inline constexpr void update_state(std::array<double, 2>& state, double r)
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
	}; 
}
