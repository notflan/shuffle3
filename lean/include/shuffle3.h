#ifndef _SHUFFLE3_H
#define _SHUFFLE3_H

#ifdef __cplusplus
#define _UNIQUE __restrict
extern "C" {
#else
#define _UNIQUE restrict
#endif

#ifdef DEBUG
#define _FORCE_INLINE __attribute__((gnu_inline)) static inline
#else
#define _FORCE_INLINE __attribute__((gnu_inline)) extern inline
#endif

struct panicinfo {
	const char* file;
	const char* function;
	int line;
};

void _do_panic(struct panicinfo pi, const char* fmt, ...) __attribute__((noreturn, cold));
#define panic(fmt, ...) _do_panic( (struct panicinfo){.file = __FILE__, .function = __func__, .line = __LINE__}, fmt __VA_OPT__(,) __VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif /* _SHUFFLE3_H */
