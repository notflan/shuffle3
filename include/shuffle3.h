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

extern const char* _prog_name;

#ifdef __cplusplus
}
#endif

#endif /* _SHUFFLE3_H */
