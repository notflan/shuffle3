#include <stdlib.h>
#include <stdio.h>

#include <string.h>


#include <shuffle3.h>
#include <panic.h>
#include <reinterpret.h>
#include <map.h>
#include <rng.h>
#include <work.h>

_Static_assert(sizeof(float)==sizeof(uint32_t), "float is not 32 bits");

int main(int argc, char** argv)
{
	//struct prog_args args = {.argc = argc, .argv = argv};
	
	//rng_test();
	
	//rng_t r = rng_new(RNG_INIT(RNG_KIND_FRNG, frng = { { 1.0, 2.0 } }));
	//rng_test_spec(r);
	//rng_free(r);

	if( argv[1] ) {
		//map_and_then(argv[1], &map_callback, &args);
		return do_work((work_args_t) {
				.op = OP_SHUFFLE_IP,
				.data.op_shuffle_ip.file = argv[1],
				});
	}

	return 0;
}

