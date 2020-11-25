#include <rng/impl.hpp>
#include <rng/frng.hpp>
#include <iostream>

namespace rng {	
	using namespace std;
	inline void test()
	{
		frng rng(1.0, 2.0);

		for(int i=0;i<10;i++) {
			double d = rng.next_double();
			long l = rng.next_long(0, 100);

			cout << "Sampled: " << d << endl; 
			cout << "Long: " << l << endl;
		}
	}

}


extern "C" void frng_test()
{
	rng::test();
}
