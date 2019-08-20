#ifndef _ARRAY_H
#define _ARRAY_H
#include <ptr_store.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct array* array_t;

array_t ar_create_file(S_LEXENV, FILE* from, size_t element_size, int owns_stream);
array_t ar_create_memory_from(S_LEXENV, const void* from, size_t element_size, size_t count);
array_t ar_create_memory(S_LEXENV, size_t size, size_t count, int init0);

const void* ar_get(const array_t this, long i);
void ar_set(array_t this, long i, const void* obj);
long ar_size(const array_t this);
long ar_full_size(const array_t this);
long ar_type(const array_t this);
int ar_get_into(const array_t this, long i, void* buffer);
void ar_swap(array_t from, long i, long j);
void ar_reinterpret(array_t from, size_t element);
int ar_dump(const array_t from, void* buffer, size_t offset, size_t count);
int ar_ndump(const array_t from, void* buffer, size_t bufsize, size_t offset, size_t count);

#define ar_get_v(this, type, i) (*((const type*)ar_get(this, i)))
#define ar_set_v(this, i, value) ar_set(this, i, &value)

#endif /* _ARRAY_H */
