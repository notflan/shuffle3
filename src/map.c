#define _GNU_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include<sys/mman.h>
#include<fcntl.h>

#define FILEMODE S_IRWXU | S_IRGRP | S_IROTH

#include <map.h>

__attribute__((pure, nonnull(1)))
static inline int _map(mmap_t* restrict map)
{
	//TODO: Based on map->len, choose (or calculate) appropriate `MAP_HUGE_*` flag
	return (map->ptr = mmap(NULL, map->len, PROT_READ | PROT_WRITE, MAP_SHARED, map->fd, 0)) != MAP_FAILED;
}
//#define _map(...) _map(__VA_ARGS__, 0)

int map_advise_rand(mmap_t* restrict ptr, int need)
{
	int flags = MADV_RANDOM | (need ? MADV_WILLNEED : 0);
	if(madvise(ptr->ptr, ptr->len, flags)) {
		perror("madvise() failed");
		return 0;
	}
	return 1;
}

int map_raw_fd(int fd, mmap_t* restrict ptr, const size_t sz[static 1])
{
	if(fd < 0) return 0;
	struct stat st;
	
	
	if(sz && ftruncate(fd, *sz)) {
		perror("Failed to allocate");
		return 0;
	} else if(fstat(fd, &st) < 0) {
		perror("Failed to stat file");
		return 0;
	}

	struct mmap map = { .fd = fd, .ptr = NULL, .len = sz ? *sz : st.st_size };

	if (!_map(&map)) {
		perror("mmap() failed");
		return 0;
	}

	*ptr = map;

	return 1;

}

int open_and_map(const char* file, mmap_t* restrict ptr)
{
	int fd;
	struct stat st;
	if ((fd = open(file, O_RDWR, FILEMODE)) < 0) {
		perror("Failed to open file");
		return 0;
	}

	if (fstat(fd, &st) < 0) {
		perror("Failed to stat file");
		close(fd);
		return 0;
	}

	struct mmap map = { .fd = fd, .ptr = NULL, .len = st.st_size };

	if (!_map(&map)) {
		perror("mmap() failed");
		close(fd);
		return 0;
	}

	*ptr = map;

	return 1;
}

int open_and_alloc(const char* file, mmap_t* restrict ptr, size_t sz)
{
	int fd;
	if ((fd = open(file, O_CREAT | O_RDWR, FILEMODE)) < 0) {
		perror("Failed to open file");
		return 0;
	}

	if(ftruncate(fd, sz))
	{
		perror("Failed to allocate");
		close(fd);
		return 0;
	}

	struct mmap map = { .fd = fd, .ptr = NULL, .len = sz };

	if (!_map(&map)) {
		perror("mmap() failed");
		close(fd);
		return 0;
	}

	*ptr = map;

	return 1;
}


inline
int unmap_and_close_s(mmap_t map, int flags)
{
	register int rval=1;
	if (map.fd && msync(map.ptr, map.len, flags))
		perror("msync() failed");
	if (munmap(map.ptr, map.len) < 0) {
		perror("munmap() failed");
		rval=0;
	}
	if (close(map.fd) <0) {
		perror("Failed to close fd");
		rval=0;
	}

	return rval;
}


int unmap_and_close(mmap_t map)
{
	return unmap_and_close_s(map, MS_SYNC /* | MS_INVALIDATE XXX: I don't think this is needed, we son't be using the mapping anymore*/);
}

__attribute__((nonnull(1)))
int dup_map(const mmap_t *in, mmap_t* restrict ptr, const size_t *new_size, enum map_flags flags)
{
	int fd;
	if ( (fd = dup(in->fd)) < 0 )
	{
		perror("dup() failed");
		return 0;
	}

	size_t sz;
	if(new_size) {
		if(ftruncate(fd, sz = *new_size))
		{
			perror("Failed to set cloned map size");
			close(fd);
			return 0;
		}
	} else	sz = in->len;
	
	struct mmap map = { .fd = fd, .ptr = NULL, .len = sz };

	if ((map.ptr = mmap(NULL, map.len, PROT_READ | (flags & MMF_READONLY ? 0 : PROT_WRITE), (flags & MMF_PRIVATE) ? MAP_PRIVATE : MAP_SHARED,fd, 0)) == MAP_FAILED) {
		perror("mmap() failed");
		close(fd);
		return 0;
	}

	*ptr = map;
	return 1;
}
