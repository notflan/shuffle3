#ifndef _DEBUG_H
#define _DEBUG_H

// If `TRACE` is defined, or if `NOTRACE` is defined, use simplified debug info
#if defined(TRACE) || !defined(NOTRACE)
#define D_TRACE
#define	_D__FUNC __PRETTY_FUNCTION__
#else
#define _D__FUNC __func__
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct debuginfo {
	const char* file;
	const char* function;
	//const char* pfunction; //TODO: for `#if defined(TRACE)`, __PRETTY_FUNCTION__ goes here, __func__ goes above.
	int line;
};

void _do_dprintf(struct debuginfo di, const char* fmt, ...);

#ifdef __cplusplus
#define _D_COMPTIME constexpr
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
#define D_dprintf(fmt, ...) do { if consteval { (void)0; } else { _real_dprintf(__FILE__, _D__FUNC, __LINE__, fmt __VA_OPT__(,) __VA_ARGS__); } } while(0)
}
#else
#define _D_COMPTIME

#ifdef DEBUG
#define D_dprintf(fmt, ...) _do_dprintf( (struct debuginfo){.file = __FILE__, .function = _D__FUNC, .line = __LINE__}, fmt __VA_OPT__(,) __VA_ARGS__)
#else
static _D_COMPTIME inline void _do__nothing(const char* fmt, ...) {}
#define D_dprintf(fmt, ...) _do__nothing(fmt __VA_OPT__(,) __VA_ARGS__)  //(fmt __VA_OPT__(,)  __VA_ARGS__, (void)0)
#endif
#endif

#ifdef __cplusplus
}
#endif
#undef _D_COMPTIME

#endif /* _DEBUG_H */
