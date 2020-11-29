#ifndef _FSVEC_H
#define _FSVEC_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#define restrict __restrict__
#endif

// A simple file-backed back inserter
typedef struct {
	FILE* backing;
	size_t len;
} fvec_t;

int fvec_new(fvec_t* restrict obj, const char* path);
void fvec_close(fvec_t* restrict obj);

int fvec_pop_end(fvec_t* restrict obj, size_t sz);
void fvec_push_whole_buffer(fvec_t* restrict obj, const void* _buffer, size_t sz);
int fvec_get_whole_buffer(const fvec_t* restrict obj, void* _buffer, size_t _sz);

#ifdef __cplusplus
#undef restrict
}
#endif

#endif /* _FSVEC_H */
