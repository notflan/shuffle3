#ifndef _PTR_STORE_H
#define _PTR_STORE_H

#include <stddef.h>
typedef struct store_link {
	void* ptr;

	int is_obj;

	struct store_link* next;
} store_t;

typedef struct store_obj {
	void* state;

	void (*constructor)(struct store_obj* this);
	void (*destructor)(struct store_obj* this);
	
	void (*copy_to)(struct store_obj* this, struct store_obj* other);
} object;

store_t *new_store();
void* store_malloc(store_t *store, size_t sz);
void store_free(store_t* store, void* ptr);
void destroy_store(store_t *store);
void* store_ptr(store_t *store, void *heap);
void store_collect(store_t *store);
store_t* _store_first_empty(store_t *store, int setobj);
object* copy_object(object* from, object* to);
object* clone_object(store_t* store, object *from);

object *new_object_unmanaged(object prototype);
void free_object_unmanaged(object* obj);
object* store_object(store_t* store, object prototype);
void free_object(store_t *store,object *obj);
object object_prototype();

extern const object OBJ_PROTO;

#define strack _store_first_empty(__internal_store, 0)->ptr = 
#define strack_obj _store_first_empty(__internal_store, 1)->ptr =

#define S_LEXENV store_t* __internal_store
#define S_LEXCALL(f, ...) f(__internal_store, __VA_ARGS__)
#define LEXENV __internal_store
#define scollect() store_collect(__internal_store)
#define smalloc(sz) store_malloc(__internal_store, sz)
#define sdelete(ptr) _Generic((ptr), object*: free_object, default: store_free)(__internal_store, ptr)
#define snew(type) ((type*)smalloc(sizeof(type)))
#define snew_obj(proto) store_object(__internal_store, proto)
#define MANAGED(var) { \
				store_t * __internal_store = new_store(); \
				var; \
				destroy_store(__internal_store); \
			}

#define STORE_LEXBIND(type, name) type name = *((type*)store_malloc(__internal_store, sizeof(type)));

#endif /* _PTR_STORE_H */
