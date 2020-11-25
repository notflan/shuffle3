
#include "impl.hpp"

namespace rng
{
	struct drng : public RNG
	{
		inline drng(std::uint32_t seed) : state(seed){sample();}
		inline drng() : drng(1){}

		static drng from_time();

		int rand();
	protected:
		double sample() override;
	private:
		std::uint32_t state;
	};
}
