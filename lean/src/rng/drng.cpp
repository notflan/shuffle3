#include <cstdlib>
#include <ctime>
#include <iostream>

#include <rng/drng.hpp>

namespace rng
{
	static inline void d_test()
	{
		using namespace std;
		drng rng(100);

		for(int i=0;i<10;i++) {
			double d = rng.next_double();
			long l = rng.next_long(-10, 10);

			std::array<bool, 10> ar;
			for(auto& i : ar) i = rng.chance();

			cout << "D Sampled: " << d << endl; 
			cout << "D Long: " << l << endl;
			cout << "D Bools: [ ";
			for(const auto& i : ar) cout << i << " ";
			cout << "]" << endl;
		}
	}

	drng drng::from_time() { return drng(time(NULL)); }

	int drng::rand()
	{
		return rand_r(&state);
	}

	double drng::sample()
	{
		int val = rand_r(&state);
		return (double)val / (double)RAND_MAX;
	}
}

extern "C" void drng_test()
{
	return rng::d_test();
}
