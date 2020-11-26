#include <shuffle3.h>
#include <work.h>
#include <panic.h>

namespace work
{
	/// Shuffle or unshuffle in place
	template<bool unshuffle>
	int xshuffle_ip(const char* file)
	{
		return 0;
	}

	/// Shuffle or unshuffle out of place
	template<bool unshuffle>
	int xshuffle_op(const char* ifile, const char* ofile, bool is_buffered)
	{
		return 0;
	}

}

int help()
{
	//Print help then exit	
	return 1;
}

extern "C" int do_work(const work_args_t args)
{
	using A = decltype(args.op);
	switch (args.op) {
		case A::OP_SHUFFLE_IP: return work::xshuffle_ip<false >(args.data.op_shuffle_ip.file);
		case A::OP_SHUFFLE_OP: return work::xshuffle_op<false >(args.data.op_shuffle_op.ifile,
						    	args.data.op_shuffle_op.ofile,
							args.data.op_shuffle_op.buffered == WORK_BO_BUFFERED);
		case A::OP_UNSHUFFLE_IP: return work::xshuffle_ip<true >(args.data.op_unshuffle_ip.file);
		case A::OP_UNSHUFFLE_OP: return work::xshuffle_op<true>(args.data.op_unshuffle_op.ifile,
						    	args.data.op_unshuffle_op.ofile,
							args.data.op_unshuffle_op.buffered == WORK_BO_BUFFERED);
		case A::OP_HELP: return help();

		default: panic("Unknown op %d", (int)args.op);
	}
	return 0;
}
