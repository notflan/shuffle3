
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#include <panic.h>
#include <map.h>



namespace {
	constexpr inline auto MEMFD_DEFAULT_FLAGS = MFD_CLOEXEC;
	int memfd_alloc(const char* name, size_t size = 0, int flags = MEMFD_DEFAULT_FLAGS)
	{
		int fd = memfd_create(name, flags);
		if(fd < 0) panic("memfd_create() failed");
		if(size)
			if(ftruncate(fd, size)) { close(fd); panic("ftruncate() failed"); }
		return fd;
	}
}

namespace mm {
	vmap::vmap()
		: mmap(create_raw_fd(memfd_alloc(__PRETTY_FUNCTION__))) {}
	vmap::vmap(size_t sz)
		: mmap(create_raw_fd(memfd_alloc(__PRETTY_FUNCTION__, sz))) {}

}
