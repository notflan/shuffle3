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

//** Features **//

//#define _FS_SPILL_BUFFER /* Use a file-backed buffer when unshuffling in cases of too high memory usage. Will cause massive slowdowns but can stop OOMs when unshuffling large files */

/// When to spill a file-backed buffer onto the fs (only used when `_FS_SPILL_BUFFER` is enabled).
#define FSV_DEFAULT_SPILL_AT ((1024 * 1024) * 10) //10MB 

/*
#ifdef _FS_SPILL_BUFFER
#define DYN 1
#if _FS_SPILL_BUFFER == DYN
#undef _FS_SPILL_BUFFER
#define _FS_SPILL_BUFFER DYN
#endif
#undef DYN
#endif
*/
//** Globals *//

extern const char* _prog_name;

#ifdef __cplusplus
}
#endif

#endif /* _SHUFFLE3_H */
