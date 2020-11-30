#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

int _can_allocate(size_t bytes)
{
	void* shim = malloc(bytes);
	if(shim) {
		free(shim);
		return 1;
	} else  return 0;
}
