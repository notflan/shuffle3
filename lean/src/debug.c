#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include <debug.h>

#include <colours.h>

void _do_dprintf(struct debuginfo info, const char* fmt, ...)
{
#ifdef DEBUG
	va_list li;
	va_start(li, fmt);
	fprintf(stderr, "["FGRN("dbg")" " FYEL("@") BOLD("%s") "->"  FWHT("%s") ":" FYEL("%d") "]: ", info.file, info.function,info.line);
	vfprintf(stderr, fmt, li);
	fprintf(stderr, "\n");
	va_end(li);
#endif
}
