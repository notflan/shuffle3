#ifndef _WORK_H
#define _WORK_H

#ifdef __cplusplus
extern "C" {
#endif

enum  work_buffer_opts {
	WORK_BO_CPIP=0, // Copy then run in place on output file (default)
	WORK_BO_BUFFERED=1, // Read whole input into memory then perform
};

typedef struct {
	enum {
		OP_SHUFFLE_IP, // Shuffle in place
		OP_SHUFFLE_OP, // Shuffle out of place

		OP_UNSHUFFLE_IP, // Unshuffle in place
		OP_UNSHUFFLE_OP, // Unshuffle out of place
	} op;
	union {
		struct {
			const char* file;
		} op_shuffle_ip;

		struct {
			enum work_buffer_opts buffered;
			const char* ifile;
			const char* ofile;
		} op_shuffle_op;
		
		struct {
			const char* file;
		} op_unshuffle_ip;

		struct {
			enum work_buffer_opts buffered;
			const char* ifile;
			const char* ofile;
		} op_unshuffle_op;
	} data;
} work_args_t;

int do_work(work_args_t args);

#ifdef __cplusplus
}
#endif

#endif /* _WORK_H */
