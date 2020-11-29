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

//

extern const char* _prog_name;

#ifdef __cplusplus
}
#endif

#endif /* _SHUFFLE3_H */
