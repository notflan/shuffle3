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
#define panic(fmt, ...) _do_panic( (struct panicinfo){.file = __FILE__, .function = __func__, .line = __LINE__}, fmt __VA_OPT__(,) __VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif
