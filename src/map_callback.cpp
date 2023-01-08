
#include <shuffle3.h>
#include <reinterpret.hpp>
#include <map.h>


extern "C" void* map_and_then(const char* file, map_cb callback, void* user)
{
	mm::mmap map(file);
	return callback(std::as_const(map).as_raw(), user);
}

extern "C" void* map_fd_and_then(int fd, map_cb callback, void* user)
{
	auto map = mm::mmap::map_raw_fd(fd);
	return callback(std::as_const(map).as_raw(), user);
}
