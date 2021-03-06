#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include <panic.h>

#include <colours.h>

__attribute__((noreturn)) void _do_panic(struct panicinfo info, const char* fmt, ...)
{
	va_list li;
	va_start(li, fmt);
	 fprintf(stderr, BOLD(UNDL(FRED("[!]"))) " (" BOLD("%s") "->" BOLD(FRED("%s")) ":" FYEL("%d") ") " BOLD(FRED("fatal error")) ": ", info.file, info.function, info.line);
	vfprintf(stderr, fmt, li);
	 fprintf(stderr, "\n");
	va_end(li);
	abort();
}
