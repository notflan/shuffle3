#include <stdio.h>
#include <float.h>
#include <stdint.h>

#include <rng.h>
#include <rng_algos.h>
#include <ptr_store.h>

/*void test_destructor(object* ptr)
{
	printf("die\n");
}*/
void _rng_object_destructor(object* obj)
{
	rng_free(obj->state);
}
RNG rng_object(S_LEXENV, RNG rng)
{
	object proto = OBJ_PROTO;
	proto.state = rng;
	proto.destructor = &_rng_object_destructor;

	return snew_obj(proto)->state;
}

void shuffle_longs(RNG with, long *data, int n)
{
	register long temp;
	for(register int i= n-1;i>0;i--)
	{
		int j = rng_next_int(with, i);
		temp = data[i];
		data[i] = data[j];
		data[j] = temp;
	}
}

void shuffle_floats(RNG with, float* data, int n)
{
	register float temp;
	for(register int i=n-1;i>0;i--)
	{
		int j = rng_next_int(with,i);
		temp = data[i];
		data[i] = data[j];
		data[j] = temp;
	}
}

void shuffle_bytes(RNG with, unsigned char* data, int n)
{
	register unsigned char temp;
	for(int i=n-1;i>0;i--)
	{
		int j = rng_next_int(with, i);
		temp  = data[i];
		data[i] = data[j];
		data[j] = temp;
	}
}

void minmax_longs(long* min, long* max, const long* data, int n)
{
	for(register int i=0;i<n;i++)
	{
		if(data[i] > *max) *max = data[i];
		if(data[i] < *min) *min = data[i];
	}
}

void minmax_floats(float* min, float* max, const float* data, int n)
{
	for(register int i=0;i<n;i++)
	{
		if(data[i] > *max) *max = data[i];
		if(data[i] < *min) *min = data[i];
	}
}

void minmax_sbytes(signed char* min, signed char* max, const signed char* data,int n)
{
	for(register int i=0;i<n;i++)
	{
		if(data[i] > *max) *max = data[i];
		if(data[i] < *min) *min = data[i];
	}
}

void print_array(unsigned char* data, int len)
{
	for(register int i=0;i<len;i++)
		printf("%d ", (int)data[i]);
	printf("\n");
}

void shuffle3(S_LEXENV, void* data, int raw_len)
{
	RNG xoro = S_LEXCALL(rng_object, rng_new(RNG_ALGO(xoroshiro128plus)));
	RNG frng = S_LEXCALL(rng_object, rng_new(RNG_ALGO(frng)));
	RNG drng = S_LEXCALL(rng_object, rng_new(RNG_ALGO(drng)));

	long min = INT64_MAX;
	long max = INT64_MIN;

	float fmin = (float)DBL_MAX;
	float fmax = (float)-DBL_MAX;

	signed char bmax = INT8_MIN;
	signed char bmin = INT8_MAX;

	minmax_longs(&min, &max, data, raw_len/sizeof(long));
	uint64_t xseed[2];
	xseed[0] = min;
	xseed[1] = max;

	rng_seed(xoro, xseed);
	shuffle_longs(xoro, data, raw_len/sizeof(long));

	minmax_floats(&fmin, &fmax, data, raw_len/sizeof(float));
	double fseed[2];
	fseed[0] = fmin;
	fseed[1] = fmax;
	rng_seed(frng, fseed);
	shuffle_floats(frng, data, raw_len/sizeof(float));

	minmax_sbytes(&bmin, &bmax, data, raw_len);
	unsigned int seed = (0xfffa << 16) | (bmin<<8) | bmax;
	rng_seed(drng, &seed);
	shuffle_bytes(drng, data, raw_len);
}

int main()
{
	MANAGED({
		void * data = smalloc(255);
		int raw_len = 255;
		for(int i=0;i<raw_len;i++)
			((unsigned char*)data)[i] = i;

		print_array(data,raw_len);

		shuffle3(LEXENV, data, raw_len);
		
		print_array(data,raw_len);

		/*char* buffer = smalloc(32);
		int* nw = snew(int);
		object proto = OBJ_PROTO;
		proto.destructor = &test_destructor;
		proto.constructor = &test_destructor;
		object* obj = snew_obj(proto);
		*nw=22;
		sprintf(buffer, "hello %d\n", *nw);
		sdelete(nw);
		sdelete(obj);
		printf("%s", buffer);*/
	});
	return 0;
}
