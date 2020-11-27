#ifndef _DEBUG_H
#define _DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif

struct debuginfo {
	const char* file;
	const char* function;
	int line;
};

void _do_dprintf(struct debuginfo di, const char* fmt, ...);

#ifdef __cplusplus
extern "C++" {
#include <utility>
	template<typename... Args>
	inline void _real_dprintf(const char* file, const char* function, int line, const char* fmt, Args&&... args)
	{
#ifdef DEBUG
		debuginfo i = { file, function, line };
		_do_dprintf(i, fmt, std::forward<Args>(args)...);
#endif
	}
#define D_dprintf(fmt, ...) _real_dprintf(__FILE__, __func__, __LINE__, fmt __VA_OPT__(,) __VA_ARGS__)
}
#else

#ifdef DEBUG
#define D_dprintf(fmt, ...) _do_dprintf( (struct debuginfo){.file = __FILE__, .function = __func__, .line = __LINE__}, fmt __VA_OPT__(,) __VA_ARGS__)
#else
static inline void _do__nothing(const char* fmt, ...) {}
#define D_dprintf(fmt, ...) _do__nothing(fmt __VA_OPT__(,) __VA_ARGS__)  //(fmt __VA_OPT__(,)  __VA_ARGS__, (void)0)
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* _DEBUG_H */
