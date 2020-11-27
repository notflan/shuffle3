#include <ptr_store.h>
#include <array.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct array {
	object* base;

	size_t size;
	size_t element;

	size_t full_size;

	void *tl_state;
	void* state;
	
	int (*dump)(const array_t this, void* buffer, size_t buffermax, size_t offset, size_t count);
	void* (*get)(struct array* this, long i);
	void (*set)(struct array* this, long i, const void* item);

	unsigned char data[];
};

static void* _ar_memory_get(struct array* this, long i)
{
	if(i< 0|| i>= this->size) return NULL;
	return &this->data[i*this->element];
}

static void _ar_memory_set(struct array* this, long i, const void* item)
{
	if(i<0 || i>= this->size) return;

	memcpy(this->data+(i*this->element), item, this->element);
}

static int _ar_memory_dump(const array_t this, void* buffer, size_t bufsize, size_t offset, size_t count)
{
	if(offset+count > this->size) return 0;
#define min(x, y) (x<y?x:y)
	long fsz = count*this->element;
	long wsz = min(fsz, bufsize);
	memcpy(buffer, this->data+(offset*this->element), wsz);
	return (wsz==fsz);
#undef min
}

static void _ar_memory_destructor(object* this)
{
	free(this->state);
	this->state=NULL;
}

array_t ar_create_memory(S_LEXENV, size_t size, size_t count, int init0)
{
	array_t mem = malloc(sizeof(struct array)+(size*count));
	memset(mem,0,sizeof(struct array));
	if(init0)
		memset(mem->data, 0, size*count);

	mem->size = count;
	mem->element = size;
	mem->full_size = size*count;

	mem->get = &_ar_memory_get;
	mem->set = &_ar_memory_set;
	mem->dump = &_ar_memory_dump;

	object obj = OBJ_PROTO;
	
	obj.state= mem;
	obj.destructor = &_ar_memory_destructor;

	return snew_obj(obj)->state;
}

array_t ar_create_memory_from(S_LEXENV, const void* from, size_t element_size, size_t count)
{
	array_t mem = ar_create_memory(LEXENV, element_size, count, 0);
	memcpy(mem->data, from, element_size*count);
	return mem;
}

static inline size_t filesize(FILE* file)
{
	size_t from = ftell(file);
	fseek(file,0,SEEK_END);
	size_t ret = ftell(file);
	fseek(file,from, SEEK_SET);

	return ret;
}

static void* _ar_file_get(struct array* this, long i)
{
	if(i<0 || i>=this->size) return NULL;

	fseek(this->state, i*this->element, SEEK_SET);
	if(this->tl_state != NULL)
		free(this->tl_state);
	void* buffer = malloc(this->element);
	this->tl_state = buffer;
	fread(buffer, this->element, 1, this->state);

	return buffer;
}

static void _ar_file_set(struct array* this, long i, const void* from)
{
	if(i<0 || i>=this->size) return;

	fseek(this->state, i*this->element, SEEK_SET);

	fwrite(from, this->element, 1, this->state);
}

static void _ar_file_destructor(object* _this)
{
	array_t this = _this->state;

	if(this->tl_state!=NULL)
		free(this->tl_state);
	if(this->data[0])
		fclose(this->state);

	this->state=NULL;
	this->tl_state=NULL;

	free(this);
}

array_t ar_create_file(S_LEXENV, FILE* from, size_t element_size, int owns_stream)
{
	array_t mem = malloc(sizeof(struct array)+1);

	memset(mem,0, sizeof(struct array));

	mem->data[0] = (unsigned char)owns_stream;

	mem->state = from;

	mem->element = element_size;
	mem->full_size = filesize(from);
	mem->size = mem->full_size/element_size;

	mem->get = &_ar_file_get;
	mem->set = &_ar_file_set;

	object obj = OBJ_PROTO;

	obj.state = mem;
	obj.destructor = &_ar_file_destructor;	

	return snew_obj(obj)->state;
}

///Generic accessor functions

const void* ar_get(const array_t this, long i)
{
	return this->get(this, i);
}

void ar_set(array_t this, long i, const void* obj)
{
	this->set(this, i, obj);
}

long ar_size(const array_t this)
{
	return this->size;
}

long ar_full_size(const array_t this)
{
	return this->full_size;
}

long ar_type(const array_t this)
{
	return this->element;
}

int ar_get_into(const array_t this, long i, void* buffer)
{
	const void* ptr = ar_get(this, i);
	if(!ptr) return 0;
	memcpy(buffer,ptr,this->element);
	return 1;
}

void ar_swap(array_t from, long i, long j)
{
	unsigned char temp[from->element];
	ar_get_into(from, i, temp);
	
	ar_set(from, i, ar_get(from, j));
	ar_set(from, j, temp);
}

void ar_reinterpret(array_t from, size_t element)
{
	long fsz = from->full_size;

	from->element = element;
	from->size = fsz/element;
}

int ar_dump(const array_t from, void* buffer, size_t offset, size_t count)
{
	if(from->dump)
		return from->dump(from, buffer, from->full_size, offset,count);
	else {
		unsigned char* buf = buffer;
		for(int i=0;i<count;i++)
		{
			if(!ar_get_into(from, offset+i, buf)) return 0;
			buf+=from->element;
		}
		return 1;
	}
}

int ar_ndump(const array_t from, void* buffer, size_t bufsize, size_t offset, size_t count)
{
	if(from->dump)
		return from->dump(from, buffer, bufsize, offset,count);
	else {
		unsigned char* buf = buffer;
		unsigned char* maxbuf = buf+bufsize;

		for(int i=0;i<count;i++)
		{
			if(buf>=maxbuf) return 0;
			if(!ar_get_into(from, offset+i, buf)) return 0;
			buf+= from->element;
		}
		return 1;
	}
}
