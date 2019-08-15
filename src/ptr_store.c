

#include <ptr_store.h>
#include <string.h>
#include <stdlib.h>

store_t *new_store()
{
	store_t *s = malloc(sizeof(store_t));
	memset(s,0,sizeof(store_t));
	return s;
}

void* store_malloc(store_t *store, size_t sz)
{
	void ** sptr;
	if(store->ptr == NULL)
		sptr = &store->ptr;
	else if (store->next == NULL) {
		store->next = new_store();
		return store_malloc(store->next, sz);
	} else {
		return store_malloc(store->next, sz);
	}

	*sptr = malloc(sz);
	memset(*sptr, 0, sz);

	return *sptr;
}

void store_free(store_t* store, void* ptr)
{
	if(store->ptr == ptr)
	{
		if(store->is_obj)
		{
			object* obj = store->ptr;
			if(obj->destructor!=NULL)
				obj->destructor(obj);
		}
		free(ptr);
		store->ptr = NULL;
		store->is_obj=0;
	} else if (store->next) {
		store_free(store->next,ptr);
	}
}

store_t* _store_first_empty(store_t *store, int obj)
{
	if(!store->ptr) { store->is_obj=obj;  return store; }
	else if(store->next) return _store_first_empty(store->next, obj);
	else {
		store->next = new_store();
		store->next->is_obj = obj;
		return store->next;
	}
}

static store_t* _store_ptr(store_t *store, void* heap)
{
	if(!store->ptr)
		store->ptr = heap;
	else if(store->next)
		return _store_ptr(store->next, heap);
	else {
		store->next = new_store();
		return _store_ptr(store->next, heap);
	}
	return store;

}

void* store_ptr(store_t *store, void *heap)
{
	if(!store->ptr)
		store->ptr = heap;
	else if(store->next)
		return store_ptr(store->next, heap);
	else {
		store->next = new_store();
		return store_ptr(store->next, heap);
	}
	return store->ptr;
}

void destroy_store(store_t *store)
{
	while(store)
	{
		if(store->ptr)
		{
			if(store->is_obj)
			{
				object *obj = store->ptr;
				if(obj->destructor!=NULL)
					obj->destructor(obj);
			}
			free(store->ptr);

		}
		store_t *next = store->next;
		
		free(store);
		store = next;
	}
}

void store_collect(store_t *store)
{
	int first=1;
	while(store)
	{
		if(store->ptr)
		{
			if(store->is_obj)
			{
				object *obj = store->ptr;
				if(obj->destructor!=NULL)
					obj->destructor(obj);
			}

			free(store->ptr);
		}
		store_t *next = store->next;
		store->is_obj=0;
		if(!first) free(store);
		first=0;
		store=next;
	}
}

object *new_object_unmanaged(object prototype)
{
	object* ret = malloc(sizeof(object));
	memcpy(ret, &prototype, sizeof(object));

	if(ret->constructor!=NULL)
		ret->constructor(ret);

	return ret;
}

void free_object_unmanaged(object* obj)
{
	if(obj->destructor!=NULL)
		obj->destructor(obj);
	free(obj);
}

object* store_object(store_t* store, object prototype)
{
	object* obj = new_object_unmanaged(prototype);

	store_t* s = _store_ptr(store, obj);
	s->is_obj = 1;

	return obj;
}

object* copy_object(object* from, object* to)
{
	to->constructor = from->constructor;
	to->destructor = from->destructor;

	to->copy_to(from, to);
	return to;
}

object* clone_object(store_t* store, object *from)
{
	object* new = malloc(sizeof(object));

	from->constructor(new);
	copy_object(from, new);

	if(store)
	{
		store_t* s = _store_ptr(store, new);
		s->is_obj = 1;
	}

	return new;
}

void free_object(store_t *store,object *obj)
{
	if(store->ptr == obj)
	{
		if(obj->destructor!=NULL)
			obj->destructor(obj);
		
		free(obj);
		store->ptr = NULL;
		store->is_obj=0;
	} else if (store->next) {
		free_object(store->next,obj);
	}

}

object object_prototype()
{
	object o;
	memset(&o, 0, sizeof(object));
	return o;
}

static void _copy_object_default(object* f, object* t)
{
	t->state = f->state;
}

const object OBJ_PROTO = {NULL,NULL,NULL, &_copy_object_default};
