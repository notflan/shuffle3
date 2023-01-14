#ifndef _ERROR_H
#define _ERROR_H

#ifndef $PASTE
#	define $_PASTE(x,y) x ## y
#	define $PASTE(x,y) $_PASTE(x,y)
#endif

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}

constexpr inline bool is_noexcept=
#if __cpp_exceptions
#define EXCEPT 1
//#define try try
//#define catch(...) catch(__VA_ARGS__)
	false
#else
#define EXCEPT 0
#define NOEXCEPT
//#define catch(...) __try {} catch(__VA_ARGS__)
//#define try if constexpr(!is_noexcept)
//#define throw (void)0
	true
#endif
	;


#endif

#endif /* _ERROR_H */
