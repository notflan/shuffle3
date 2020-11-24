
#include <shuffle3.h>
#include <reinterpret.hpp>
#include <map.h>


extern "C" void* map_and_then(const char* file, map_cb callback, void* user)
{
	mm::mmap map(file);
	return callback(map.as_raw(), user);
}
