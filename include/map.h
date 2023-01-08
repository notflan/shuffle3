#ifndef _MAP_H
#define _MAP_H

#ifdef __cplusplus
#include <stdexcept>
extern "C" {
#define restrict __restrict__
#define MIFCXX(y, n) y
#define MIFC(y, n) n 
#else
#define MIFCXX(y, n) n
#define MIFC(y, n) y 
#endif

#include <stddef.h>

typedef struct mmap {
	int fd;

	void* ptr;
	size_t len;
} mmap_t;

enum map_flags {
	MMF_SHARED,
	MMF_PRIVATE,
	MMF_READONLY,
};

int map_raw_fd(int fd, mmap_t* restrict ptr, const size_t MIFCXX(* sz, sz[static 1]));
int open_and_alloc(const char* file, mmap_t* restrict ptr, size_t sz);
int open_and_map(const char* file, mmap_t* restrict ptr);
int unmap_and_close(mmap_t map);
int dup_map(const mmap_t *in, mmap_t* restrict out, const size_t *new_size, enum map_flags flags) __attribute__((nonnull(1)));
int map_advise_rand(mmap_t* restrict ptr, int need);

typedef void* (*map_cb)(mmap_t map, void* user);
void* map_and_then(const char* file, map_cb callback, void* user);
void* map_fd_and_then(int fd, map_cb callback, void* user);

#ifdef __cplusplus
}
#include <panic.h>
#include "reinterpret.h"
#include <cstdint>
namespace mm {
	enum class Access {
		Random,
	};
	struct mmap;
	template<typename T>
	concept MemMap = std::derived_from<T, mmap>
			and std::is_constructible_v<T, mmap_t>;
	struct mmap {
		template<MemMap M = mmap>
		inline static M allocate(const char* file, std::size_t sz)
		{
			mmap_t map;
			if(!open_and_alloc(file, &map, sz)) panic("Failed to allocmap file");
			return M{map};
		}
		inline static mmap_t create_raw(const char* file)
		{
			mmap_t map;
			if (!::open_and_map(file, &map)) panic("Failed to map file");
			return map;
		}
		inline static mmap_t create_raw_fd(int fd, size_t sz)
		{
			mmap_t map;
			if (!::map_raw_fd(fd, &map, &sz)) panic("Failed to allocmap fd");
			return map;
		}
		inline static mmap_t create_raw_fd(int fd)
		{
			mmap_t map;
			if (!::map_raw_fd(fd, &map, nullptr)) panic("Failed to map fd");
			return map;
		}
		template<MemMap M = mmap>
		inline static M map_raw_fd(int fd, std::size_t sz = 0)
		{
			if(sz)
				return M{fd, sz};
			else	return M{fd};
		}

		inline explicit mmap(mmap_t raw) noexcept :inner(raw){}
		inline mmap(const char* file)
			: inner(create_raw(file)) {}

		inline mmap(mmap&& move) : inner(move.inner)
		{
			auto other = const_cast<mmap_t*>(&move.inner);
			other->ptr = nullptr;
		}
		inline mmap(const mmap& copt) = delete;

		inline virtual ~mmap()
		{
			if (inner.ptr) {
				::unmap_and_close(inner);
			}
		}

		inline virtual mmap&& access(Access a, bool need=false) && noexcept
		{
			if(inner.ptr)
				static_cast<mmap&>(*this).access(a, need);
			return std::move(*this);
		}
		inline virtual mmap& access(Access a, bool need=false) & noexcept
		{
			switch(a) {
				case Access::Random: ::map_advise_rand(const_cast<mmap_t*>(&inner), int(need));
				default: break;
			}
			return *this;
		}

		inline const span<const unsigned char> as_span() const noexcept { return span(as_ptr(), size()); }
		inline span<unsigned char> as_span() noexcept { return span(as_ptr(), size()); }

		inline virtual const std::uint8_t* as_ptr() const noexcept { return static_cast<const std::uint8_t*>(inner.ptr); }
		inline virtual std::uint8_t* as_ptr() noexcept { return static_cast<std::uint8_t*>(inner.ptr); }

		inline virtual std::size_t size() const noexcept { return inner.len; }

		inline virtual int as_fd() const { return inner.fd; }
		inline virtual const mmap_t& as_raw() const noexcept { return inner; }
	protected:
		inline mmap(int fd, size_t sz)
			: inner(create_raw_fd(fd, sz)) {}
		inline explicit mmap(int fd)
			: inner(create_raw_fd(fd)) {}
		inline virtual mmap_t& as_raw() noexcept { return const_cast<mmap_t&>( inner ); }
	private:	
		const mmap_t inner;
	};
	//template mmap::map_raw_fd<mmap>(int, size_t); // Probably not needed?
	//template mmap::allocate<mmap>(int, size_t);

	struct vmap : public mmap {
		inline explicit vmap(mmap_t m) noexcept : mmap(m) {}

		vmap();
		vmap(size_t);
		inline explicit vmap(int fd) noexcept : mmap(fd) {}
		//vmap(const char* file);

		//int as_fd() const override; unneeded

		inline virtual ~vmap() { /* unmap_and_close() is called by super */ }
		//TODO: Implement this ^ with `memfd_create()`, etc.
	};

	
	template<MemMap M, MemMap D = M>
	inline D dup_map(const M& map, size_t resize, map_flags flags =0)
	{
		const mmap& m = static_cast<const mmap&>(map);
		mmap_t out;
		if(! ::dup_map(&m.as_raw(), &out, &resize, flags)) panic("Failed to duplicate mapping");
		return D{out};
	}
	template<MemMap M, MemMap D = M>
	inline D dup_map(const M& map, map_flags flags =0)
	{
		const mmap& m = static_cast<const mmap&>(map);
		mmap_t out;
		if(! ::dup_map(&m.as_raw(), &out, nullptr, flags)) panic("Failed to duplicate mapping");
		return D{out};
	}
#if 0
	// dup_map() deduction guids (XXX: Are these needed, or even valid?)
	template<MemMap D>
	dup_map(const mmap& m, size_t s, map_flags f) -> dup_map<mmap, D>;

	template<>
	dup_map(const mmap& m, size_t s, map_flags f) -> dup_map<mmap, mmap>;

	template<MemMap M>
	dup_map(const M& m, size_t s, map_flags f) -> dup_map<M, M>;

	template<MemMap D>
	dup_map(const mmap& m, map_flags f) -> dup_map<mmap, D>;

	template<>
	dup_map(const mmap& m, map_flags f) -> dup_map<mmap, mmap>;

	template<MemMap M>
	dup_map(const M& m, map_flags f) -> dup_map<M, M>;
#endif
}

#undef restrict
#undef MIFCXX
#undef MIFC
#endif

#endif /* _MAP_H */
