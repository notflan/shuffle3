#include <iostream>
#include <rng.h>

namespace rng {
	void test_algo(RNG&& rng)
	{
		using namespace std;

		for(int i=0;i<10;i++) {
			double d = rng.next_double();
			long l = rng.next_long(-10, 10);

			std::array<bool, 10> ar;
			for(auto& i : ar) i = rng.chance();

			cout << "\t(Sampled: " << d; 
			cout << ", Long: " << l;
			cout << ", Bools: [ ";
			for(const auto& i : ar) cout << i << " ";
			cout << "])" << endl;
		}
	}
}

extern "C" void rng_test()
{
	using namespace std;
	
	cout << "frng:" << endl;
	rng::test_algo(rng::frng(1.0, 1.2));

	cout << "drng:" << endl;
	rng::test_algo(rng::drng(10));

	cout << "xoroshiro128+" << endl;
	rng::test_algo(rng::xoroshiro128plus(100ul, 200ul));
}
