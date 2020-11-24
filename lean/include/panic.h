#ifndef _PANIC_H
#define _PANIC_H

#ifdef __cplusplus
extern "C" {
#endif

struct panicinfo {
	const char* file;
	const char* function;
	int line;
};

void _do_panic(struct panicinfo pi, const char* fmt, ...) __attribute__((noreturn));

#ifdef __cplusplus
extern "C++" {
#include <utility>
	template<typename... Args>
	__attribute__((noreturn)) inline void _real_panic(const char* file, const char* function, int line, const char* fmt, const Args&... args)
	{
		panicinfo i = { file, function, line };
		_do_panic(i, fmt, std::forward<Args>(args)...);
	}
#define panic(fmt, ...) _real_panic(__FILE__, __func__, __LINE__, fmt __VA_OPT__(,) __VA_ARGS__)
}
#else
#define panic(fmt, ...) _do_panic( (struct panicinfo){.file = __FILE__, .function = __func__, .line = __LINE__}, fmt __VA_OPT__(,) __VA_ARGS__)
#endif

#ifdef __cplusplus
}
#endif

#endif
