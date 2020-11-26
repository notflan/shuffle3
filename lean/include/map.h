#ifndef _MAP_H
#define _MAP_H

#ifdef __cplusplus
extern "C" {
#define restrict __restrict__
#endif

#include <stddef.h>

typedef struct mmap {
	int fd;

	void* ptr;
	size_t len;
} mmap_t;

int open_and_map(const char* file, mmap_t* restrict ptr);
int unmap_and_close(mmap_t map);

typedef void* (*map_cb)(mmap_t map, void* user);
void* map_and_then(const char* file, map_cb callback, void* user);

#ifdef __cplusplus
}
#include <panic.h>
#include "reinterpret.h"
#include <cstdint>
namespace mm {
	struct mmap {
		inline static mmap_t create_raw(const char* file)
		{
			mmap_t map;
			if (!open_and_map(file, &map)) panic("Failed to map file");
			return map;
		}

		inline mmap(mmap_t raw) :inner(raw){}
		inline mmap(const char* file)
			: inner(create_raw(file)) {}

		inline mmap(mmap&& move) : inner(move.inner)
		{
			auto other = const_cast<mmap_t*>(&move.inner);
			other->ptr = nullptr;
		}
		inline mmap(const mmap& copt) = delete;

		inline ~mmap()
		{
			if (inner.ptr) {
				::unmap_and_close(inner);
			}
		}

		inline const span<const unsigned char> as_span() const { return span(as_ptr(), size()); }
		inline span<unsigned char> as_span() { return span(as_ptr(), size()); }

		inline const std::uint8_t* as_ptr() const { return (const std::uint8_t*)inner.ptr; }
		inline std::uint8_t* as_ptr() { return (std::uint8_t*)inner.ptr; }

		inline std::size_t size() const { return inner.len; }

		inline int as_fd() const { return inner.fd; }
		inline const mmap_t& as_raw() const { return inner; }
	private:	
		const mmap_t inner;
	};
}

#undef restrict
#endif

#endif /* _MAP_H */
