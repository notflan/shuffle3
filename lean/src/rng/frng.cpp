#include <rng/impl.hpp>
#include <rng/frng.hpp>
#include <iostream>

namespace rng {	
	using namespace std;
	static inline void f_test()
	{
		frng rng(1.0, 2.0);

		for(int i=0;i<10;i++) {
			double d = rng.next_double();
			long l = rng.next_long(-10, 10);

			std::array<bool, 10> ar;
			for(auto& i : ar) i = rng.chance();

			cout << "Sampled: " << d << endl; 
			cout << "Long: " << l << endl;
			cout << "Bools: [ ";
			for(const auto& i : ar) cout << i << " ";
			cout << "]" << endl;
		}
	}
}


extern "C" void frng_test()
{
	rng::f_test();
}
