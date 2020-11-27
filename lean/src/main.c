#include <stdlib.h>
#include <stdio.h>

#include <string.h>


#include <shuffle3.h>
#include <panic.h>
#include <reinterpret.h>
#include <map.h>
#include <rng.h>
#include <work.h>

#include <debug.h>

#define noreturn __attribute__((noreturn)) void

_Static_assert(sizeof(float)==sizeof(uint32_t), "float is not 32 bits");

const char* _prog_name;

noreturn help_then_exit()
{
	fprintf(stderr, "Try passing `-h`\n");
	exit(1);
}

void usage()
{
	printf( "shuffle3 - 3 pass binary shuffler\n"
		"Usage: %s -s <file>\n"
		"Usage: %s -u <file>\n", _prog_name, _prog_name);
	printf("\nOPTIONS:\n"
			"  -s\tShuffle file in place\n"
			"  -u\tUnshuffle file in place\n");
}

int main(int argc, char** argv)
{
	_prog_name = argv[0];

	work_args_t parsed;

	if( !argv[1] || *(argv[1]) != '-') help_then_exit(); 
	
	D_dprintf("Parsing `%c'", argv[1][1]);
	switch(argv[1][1])
	{
		case 's': 
			parsed.op = OP_SHUFFLE_IP;
			if(!(parsed.data.op_shuffle_ip.file = argv[2]))
			{
				fprintf(stderr, "Error: -s expected file argument.\n");
				return 1;
			}
			D_dprintf("parsed.op = %d", OP_SHUFFLE_IP);
			break;
		case 'u':
			parsed.op = OP_UNSHUFFLE_IP;
			if(!(parsed.data.op_unshuffle_ip.file = argv[2]))
			{
				fprintf(stderr, "Error: -u expected file argument.\n");
				return 1;
			}
			D_dprintf("parsed.op = %d", OP_UNSHUFFLE_IP);
			break;
		case 'h':
			usage();
			return 0;
		default:
			fprintf(stderr, "Error: unknown argument `%s'\n\n", argv[1]);
			help_then_exit();
			panic("Unreachable");
	}

	return do_work(parsed);
}

