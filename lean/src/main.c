#include <stdlib.h>
#include <stdio.h>

#include <string.h>


#include <shuffle3.h>
#include <panic.h>
#include <reinterpret.h>
#include <map.h>

_Static_assert(sizeof(float)==sizeof(uint32_t), "float is not 32 bits");

/*
static void shuffle_file(const char* filename)
{
	panic("unimplemented");
}

static void unshuffle_file(const char* filename)
{
	panic("unimplemented");
}*/

static void* map_callback(mmap_t map)
{
	printf("fd %d mapped (sz %lu) (ptr %p)\n", map.fd, map.len, map.ptr);	


	return NULL;
}

static void do_test()
{
	char* string = "Hello world.. how are you?????";
	size_t string_sz = strlen(string);
	size_t outsz;

	uint64_t* string_as_longs = bytes_to_long((uint8_t*)string, string_sz, &outsz);

	printf("%s (%lu) -> u64 %lu\n", string, string_sz, outsz);
	for (register int i=0;i<outsz;i++) {
		printf(" %lu\n", string_as_longs[i]);
	}
	printf("OK!\n\n");

	float* string_as_floats = bytes_to_float((uint8_t*)string, string_sz, &outsz);
	printf("%s (%lu) -> f32 %lu\n", string, string_sz, outsz);
	for (register int i=0;i<outsz;i++) {
		printf(" %f\n", string_as_floats[i]);
	}
	printf("OK!\n\n");
}

int main(int argc, char** argv)
{
	do_test();
	if( argv[1] ) {
		map_and_then(argv[1], &map_callback);
	}

	return 0;
}
