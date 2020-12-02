#include <filesystem>

#include <fsvec.hpp>
#include <uuid.hpp>
#include <fsvec.h>
#include <debug.h>

#include <tempfile.hpp>

#define FB file_back_buffer


struct FB::impl
{
	size_t cap;
	temp_file file;

	fvec_t backing;
};

FB::FB(size_t cap) : inner(std::make_unique<FB::impl>())
{
	// Set cap
	inner->cap = cap;
	// Create then open `inner->file`
	if(!fvec_new(&inner->backing, inner->file->c_str())) panic("Failed to open backing for temp file buffer");

}
FB::FB(file_back_buffer&& m) : inner(std::move(m.inner)){}
FB::FB() : FB(DEFAULT_CAP){}
FB::~FB()
{
	fvec_close(&inner->backing);
}

void FB::push_buf(byte* buf, size_t len)
{
	fvec_push_whole_buffer(&inner->backing, (void*)buf, len);
}

bool FB::back(byte* buf, size_t len) const
{
	return !!fvec_get_whole_buffer(&inner->backing, (void*)buf, len);	
}

bool FB::pop_n(size_t len)
{
	return !!fvec_pop_end(&inner->backing, len);
}

extern "C" void _fb_run_tests()
{
	fixed_spill_vector<int, 8> test;
	int r0,r1=0;
	for(int i=0;i<10;i++) {
		D_dprintf("push: %d", (10-i));
		test.push_back(10-i);
		r1+= (10-i);
	}
	D_dprintf("r1: %d", r1);
	r0=0;
	while(test.size())
	{
		r0+=test.back();
		D_dprintf("back: %d", test.back());
		test.pop_back();
	}
	D_dprintf("r0: %d", r0);

	if(r0!=r1) panic("fb failed test: %d, %d", r0, r1);

	D_dprintf("test successful");
}
