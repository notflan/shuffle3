#include <stdio.h>
#include <float.h>
#include <stdint.h>
#include <string.h>

#include <rng.h>
#include <rng_algos.h>
#include <ptr_store.h>
#include <array.h>

_Static_assert(sizeof(float)==4, "Float must be 32 bits.");

int surpress_out=0;

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

void shuffle(RNG with, array_t data)
{
	if(!surpress_out) printf("  -> shuffling %d objects...", (int)ar_size(data));
	for(int i=ar_size(data)-1;i>0;i--)
	{
		int j = rng_next_int(with, i);
		ar_swap(data, i, j);
	}
	if(!surpress_out) printf(" Okay\n");
}

void unshuffle(RNG with, array_t data)
{
	int rng_values[ar_size(data)-1];
	int k=0;
	if(!surpress_out) printf("  -> unshuffling %d objects...", (int)ar_size(data));
	for(int i=ar_size(data)-1;i>0;i--)
		rng_values[k++] = rng_next_int(with, i);

	for(int i=1;i<ar_size(data);i++)
	{
		ar_swap(data, i, rng_values[--k]);
	}
	if(!surpress_out) printf(" Okay\n");
}

void minmax_int64_ts(int64_t* min, int64_t* max, const array_t data)
{
	for(register int i=0;i<ar_size(data);i++)
	{
		if(ar_get_v(data, int64_t, i)  > *max) *max = ar_get_v(data, int64_t, i);
		if(ar_get_v(data, int64_t, i) < *min) *min = ar_get_v(data, int64_t, i);
	}
}
int valid_float(float f)
{
	return !( (f!=f) || (f< -FLT_MAX || f> FLT_MAX));

}
void minmax_floats(float* min, float* max, const array_t data)
{
	for(register int i=0;i<ar_size(data);i++)
	{
		if(!valid_float(ar_get_v(data,float,i))) continue;
		if(ar_get_v(data, float, i)  > *max) *max = ar_get_v(data, float, i);
		if(ar_get_v(data, float, i) < *min) *min = ar_get_v(data, float, i);
	}
}

void minmax_sbytes(int8_t* min, int8_t* max, const array_t data)
{
	for(register int i=0;i<ar_size(data);i++)
	{
		if(ar_get_v(data, int8_t, i)  > *max) *max = ar_get_v(data, int8_t, i);
		if(ar_get_v(data, int8_t, i) < *min) *min = ar_get_v(data, int8_t, i);
	}
}

void print_array(const array_t data)
{
	printf("---%d elements---\n", (int)ar_size(data));
	for(register int i=0;i<ar_size(data);i++)
		printf("%d ", (int)ar_get_v(data, unsigned char, i));
	printf("\n---\n");
}

void unshuffle3(S_LEXENV, array_t data)
{
	if(ar_type(data)!=sizeof(int8_t))
		ar_reinterpret(data, sizeof(int8_t));
	RNG xoro = S_LEXCALL(rng_object, rng_new(RNG_ALGO(xoroshiro128plus)));
	RNG frng = S_LEXCALL(rng_object, rng_new(RNG_ALGO(frng)));
	RNG drng = S_LEXCALL(rng_object, rng_new(RNG_ALGO(drng)));

	int8_t bmax = INT8_MIN;
	int8_t bmin = INT8_MAX;
	minmax_sbytes(&bmin, &bmax, data);
	uint32_t seed = (0xfffa << 16) | (bmin<<8) | bmax;
	rng_seed(drng, &seed);
	unshuffle(drng, data);

	float fmin = (float)DBL_MAX;
	float fmax = (float)-DBL_MAX;
	ar_reinterpret(data, sizeof(float));
	minmax_floats(&fmin, &fmax, data);
	double fseed[2];
	fseed[0] = fmin;
	fseed[1] = fmax;
	rng_seed(frng, fseed);
	unshuffle(frng, data);
	
	int64_t min = INT64_MAX;
	int64_t max = INT64_MIN;
	ar_reinterpret(data, sizeof(int64_t));
	minmax_int64_ts(&min, &max, data);
	uint64_t xseed[2];
	xseed[0] = min;
	xseed[1] = max;
	rng_seed(xoro, xseed);
	unshuffle(xoro, data);
}
void shuffle3(S_LEXENV, array_t data)
{
	if(ar_type(data)!=sizeof(int64_t))
		ar_reinterpret(data, sizeof(int64_t));
	RNG xoro = S_LEXCALL(rng_object, rng_new(RNG_ALGO(xoroshiro128plus)));
	RNG frng = S_LEXCALL(rng_object, rng_new(RNG_ALGO(frng)));
	RNG drng = S_LEXCALL(rng_object, rng_new(RNG_ALGO(drng)));

	int64_t min = INT64_MAX;
	int64_t max = INT64_MIN;

	float fmin = (float)DBL_MAX;
	float fmax = (float)-DBL_MAX;

	int8_t bmax = INT8_MIN;
	int8_t bmin = INT8_MAX;

	minmax_int64_ts(&min, &max, data);
	uint64_t xseed[2];
	xseed[0] = min;
	xseed[1] = max;

	rng_seed(xoro, xseed);
	shuffle(xoro, data);

	ar_reinterpret(data, sizeof(float));
	minmax_floats(&fmin, &fmax, data);

	double fseed[2];
	fseed[0] = fmin;
	fseed[1] = fmax;
	rng_seed(frng, fseed);
	shuffle(frng, data);

	ar_reinterpret(data,sizeof(int8_t));
	minmax_sbytes(&bmin, &bmax, data);
	uint32_t seed = (0xfffa << 16) | (bmin<<8) | bmax;
	rng_seed(drng, &seed);
	shuffle(drng, data);
}

void print_usage(char** argv)
{
	printf("Usage: %s -[b]s <file> [<outfile>]\nUsage: %s -[b]u <file> [<outfile>]\n", argv[0], argv[0]);
	printf("Usage: %s -S <string>|-\nUsage: %s -U <string>|-\n", argv[0], argv[0]);
	printf(" <file>\t\tFile to shuffle\n");
	printf(" <outfile>\tOutput file (only valid for buffered mode)\n");
	printf(" <string>\tThe string to shuffle (for -(S|U)). If `-`, read from stdin\n");
	printf("\ts\tShuffle\n");
	printf("\tu\tReverse shuffle\n");
	printf("\tS\tShuffle string\n");
	printf("\tU\tReverse shuffle string\n");
	printf("\n\tb\tUse buffered mode instead of shuffling in place (must specify output file)\n");
}

array_t read_whole_file(S_NAMED_LEXENV(base), FILE* fp)
{
	fseek(fp,0,SEEK_END);
	int64_t sz = ftell(fp);
	fseek(fp,0,SEEK_SET);

	array_t ar;
	MANAGED({
		void* buf = smalloc(sz);
		fread(buf, 1, sz, fp);
		ar = ar_create_memory_from(base, buf, 1, sz);
	});
	return ar;
}

#define BUFSIZE 512
#define BUFMIN 128

char* read_stdin()
{
	char* input, *p;
	int len,remain,n,size;

	size = BUFSIZE;
	input = malloc(size);
	memset(input,0,size);
	len=0;
	remain = size;

	while(!feof(stdin))
	{
		if(remain<= BUFMIN)
		{
			remain+=size;
			size *=2;
			p = realloc(input, size);
			if(p==NULL) {
				free(input);
				return NULL;
			}
			input = p;
		}
		fgets(input+len, remain, stdin);
		n+=strlen(input+len);
		len+=n;
		remain-=n;
	}
	return input;
}

int string_shuffle(char* string, int un)
{
	if(!string)
	{
		char* ptr = read_stdin();
		if(!ptr) { 
			printf("! read from stdin failed\n");
			return -1;
		}
		int ret = string_shuffle(ptr, un);
		free(ptr);
		return ret;
	}
	else if(strcmp(string, "-") == 0)
	{
		return string_shuffle(NULL, un);
	}
	else {
		surpress_out=1;
		MANAGED({
			array_t ar = ar_create_memory_from(LEXENV, string, 1, strlen(string));

			if(un) unshuffle3(LEXENV, ar);
			else shuffle3(LEXENV, ar);

			ar_reinterpret(ar, sizeof(char));

			for(int i=0;i<ar_size(ar);i++)
			{
				printf("%c", ar_get_v(ar, char, i));
			}
		});
		return 0;
	}
}

int main(int argc, char** argv)
{
	int is_buffered=0;
	int i=1;
	int is_unshuffling =0;
	if(!argv[1] || !argv[2] || argv[1][0] != '-')
	{
		print_usage(argv);
		return 1;	
	}
	else {
do_switch:
		switch(argv[1][i])
		{
			case 'b':
				if(is_buffered)
				{
					print_usage(argv);
					return 1;
				}
				is_buffered = 1;
				i+=1;
				goto do_switch;
			case 'u':
				is_unshuffling=1;
				break;
			case 's':
				break;
			case 'S':
				return string_shuffle(argv[2], 0);
			case 'U':
				return string_shuffle(argv[2], 1);
			default:
				print_usage(argv);
				return 1;
		}
	}

	if(is_buffered && !argv[3])
	{
		printf("Buffered option requires an output file\n");
		return 1;
	}

	MANAGED_RETURNABLE(int ,{
		array_t array;
	       
		if(is_buffered)
		{
			FILE* infile = fopen(argv[2], "rb");
			if(!infile)
			{
				printf("! could not open file for reading\n");
				MANAGED_RETURN(1);
			}
			array = read_whole_file(LEXENV, infile);
			fclose(infile);	
			printf("  buffered file (%ld bytes)\n", ar_size(array));
		}
		else {
			FILE* infile = fopen(argv[2], "r+b");
			if(!infile)
			{
				printf("! could not open file for reading+writing\n");
				MANAGED_RETURN(1);
			}
			array = ar_create_file(LEXENV, infile, 1, 1);
		}

		//print_array(array);

		if(is_unshuffling) unshuffle3(LEXENV, array);
		else shuffle3(LEXENV, array);
	
		if(is_buffered)
		{
			FILE* outfile = fopen(argv[3], "wb");
			if(!outfile)
			{
				printf("! could not open outfile for writing\n");
				MANAGED_RETURN(1);
			}
			void* wbuf = smalloc(ar_full_size(array));
			if(!ar_ndump(array, wbuf, ar_full_size(array), 0, ar_size(array)))
				printf("W memory dump failed, continuing anyway\n");
			fwrite(wbuf, 1, ar_full_size(array), outfile);
			fclose(outfile);
			printf("  write completed (%d bytes)\n", (int)ar_size(array));
		}
	});
	return 0;
}
