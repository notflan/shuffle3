#include <unistd.h>
#include <fsvec.h>
#include <panic.h>

int fvec_new(fvec_t* restrict obj, const char* path)
{
	obj->backing = fopen(path, "wb");
	if(!obj->backing) {
		perror("fvec_new(): Failed to open file");
		return 0;
	}
	obj->len=0;
	return 1;
}

void fvec_close(fvec_t* restrict obj)
{
	if(obj->backing) fclose(obj->backing);
	obj->backing=NULL;
	obj->len=0;
}

int fvec_pop_end(fvec_t* restrict obj, size_t sz)
{
	if(!obj->len) return 0;

	if(ftruncate(fileno(obj->backing), (obj->len-=sz))<0) {
		perror("Failed to pop buffer");
		return 0;
	}
	return 1;
}

void fvec_push_whole_buffer(fvec_t* restrict obj, const void* _buffer, size_t sz)
{
	size_t w= 0;
	size_t c;
	const unsigned char* buffer = _buffer;
	while ( w<sz && (c=fwrite(buffer+w, 1, sz-w, obj->backing))>0 ) w+=c;
	if(w!=sz) {
		perror("Corrupted buffer state, aborting");
		panic("Cannot continue");
	}
	obj->len += sz;
}
int fvec_get_whole_buffer(const fvec_t* restrict obj, void* _buffer, size_t _sz)
{
	ssize_t sz = (ssize_t)_sz;
	if(_sz != (size_t)sz) panic("Buffer size %lu too large", _sz);

	if(obj->len<(size_t)sz) return 0;
	if(fseek(obj->backing, -sz, SEEK_END)<0) {
		perror("fvec_get_whole_buffer() failed to seek");
		return 0;
	}

	size_t w=0;
	ssize_t c;
	unsigned char* buffer = _buffer;
	while ( w<_sz && (c=fread(buffer+w, 1, sz-w, obj->backing))>0 ) w+=c;
	if (w!=_sz) {
		perror("Corrupted buffer state, aborting");
		panic("Cannot continue");
	}

	return 1;
}
