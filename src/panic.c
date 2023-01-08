#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>

#include <panic.h>

#include <colours.h>

#ifndef DEFAULT_PANIC_HANDLER
#if __cpp_exceptions || 1 /* XXX: Cannot detect this here */
#	define DEFAULT_PANIC_HANDLER "_panic__start_unwind_cxx"
#else
#	define DEFAULT_PANIC_HANDLER "_panic__start_unwind_abort"
#endif
#endif


extern void _panic__start_unwind_cxx(void);
static void _panic__start_unwind_abort(void*)
__attribute__((weakref("abort"), noreturn));

static void _panic__start_unwind(void* payload)
__attribute__((noreturn, weakref(DEFAULT_PANIC_HANDLER)));

__attribute__((noreturn, weak)) void _do_panic(struct panicinfo info, const char* fmt, ...)
{
	va_list li;
	va_start(li, fmt);
#define FMT_TTY BOLD(UNDL(FRED("[!]"))) " (" BOLD("%s") "->" BOLD(FRED("%s")) ":" FYEL("%d") ") " BOLD(FRED("fatal error")) ": "
#define FMT_FIL "[!]" " (" "%s" "->" "%s" ":" "%d" ") " "fatal error" ": "
	 fprintf(stderr, isatty(fileno(stderr)) 
			? FMT_TTY
			: FMT_FIL , info.file, info.function, info.line);
	vfprintf(stderr, fmt, li);
	 fprintf(stderr, "\n");
	va_end(li);

	_panic__start_unwind(&info);
	__builtin_unreachable();
}
