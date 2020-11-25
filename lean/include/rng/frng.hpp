
#include "impl.hpp"

namespace rng
{
	struct frng : public RNG 
	{
		inline frng(double s1, double s2) : state({s1, s2}){}
		inline frng(const std::array<double, 2>& ar) : state(ar){}
		inline frng(std::array<double, 2>&& ar) : state(ar){}
		inline frng(const double (&ar)[2]) : state({ar[0], ar[1]}) {}
	protected: 
		double sample();
	private:
		std::array<double, 2> state;
	}; 
}
