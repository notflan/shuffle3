#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#include <debug.h>

#include <colours.h>

enum jt_comp {
	JT_COMP_UNIX,
	JT_COMP_DATE,
	JT_COMP_TIME,
	JT_COMP_DATETIME,
};

#define CLONE(p) ({  __auto_type $__p = (p); \
			__auto_type $__r = __builtin_choose_expr(__builtin_classify_type($__p) == 5, *$__p, $__p); \
			/* ((__typeof__($__r) []) { $__r }); }) */ \
			__typeof__($__r) $__rp[1]; \
			$__rp[0] = $__r; \
			&$__rp[0]; })

__attribute__((returns_nonnull, nonnull(1)))
static inline
const char* jtime(const struct tm* restrict lt, enum jt_comp kind)
{
	_Thread_local static char datebuf[1
			+ 4 + 2 + 4 + 1 // `dd/mm/yyyy`
			+ 6 + 3 // ` hh:MM:ss`
	];
	_Static_assert(sizeof(datebuf) != sizeof(char*), "Unexpected decay");

	//memset(datebuf, '0', sizeof(datebuf)-1);
	if(!kind)
		snprintf(datebuf, sizeof(datebuf), "%lu", mktime(CLONE(lt)));
	else {
		size_t w = 0, sz = sizeof(datebuf);
		if(kind & JT_COMP_DATE)
		w += snprintf(datebuf, sz, "%02u/%02u/%4.4u"
			, (unsigned)lt->tm_mday
			, (unsigned)lt->tm_mon
			, (unsigned)(1900 + lt->tm_year));

		if(kind & JT_COMP_TIME)
		w += snprintf(datebuf+w, sz - w, " %02u:%02u:%02u"
			, (unsigned)lt->tm_hour
			, (unsigned)lt->tm_min
			, (unsigned)lt->tm_sec);
	}
	return datebuf;
}

void _do_dprintf(struct debuginfo info, const char* fmt, ...)
{
#ifdef DEBUG
	va_list li;
	
	time_t utime;
	struct tm ltime;
	time(&utime);
	localtime_r(&utime, &ltime);

	va_start(li, fmt);

#ifdef D_TRACE
#define FMT_FUNC "%s"
#else
#define	FMT_FUNC "%s()"
#endif

#define TTY_OUT "["FGRN("dbg")" (%s %s) " FYEL("@") BOLD("%s") "->"  FWHT(FMT_FUNC) ":" FYEL("%d") "]: "
#define NCOL(x) x
#define FIL_OUT "["NCOL("dbg")" (%s %s) " NCOL("@") NCOL("%s") "->"  NCOL(FMT_FUNC) ":" NCOL("%d") "]: "

	fprintf(stderr, isatty(fileno(stderr)) ? (TTY_OUT) : (FIL_OUT)
			, jtime(&ltime, JT_COMP_DATETIME), ltime.tm_zone
			, info.file
			, info.function
			, info.line);
	vfprintf(stderr, fmt, li);
	fprintf(stderr, "\n");
	va_end(li);
#endif
}
