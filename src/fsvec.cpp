#include <filesystem>

#include <fsvec.hpp>
#include <uuid.hpp>
#include <fsvec.h>
#include <debug.h>

#define FB file_back_buffer
using std::size_t;
namespace fs = std::filesystem;

/// A temporary file name
struct temp_file
{
	inline temp_file(const temp_file& c) = delete;

	inline temp_file(temp_file&& m) : name(std::move(m.name)) { m._full_path.clear(); }
	inline temp_file() : name(uuid::generate().to_string()+"-s3"){}
	inline temp_file(const char* name) : name(name) {}
	inline temp_file(std::string&& name) : name(name) {}

	inline ~temp_file()
	{
		if(!_full_path.empty() && fs::exists(_full_path) ) {
			D_dprintf("tfile removing: %s", _full_path.c_str());
			fs::remove(_full_path);
		}
	}
	
	inline const fs::path& full_path() const
	{
		if(_full_path.empty()) {
			_full_path = fs::absolute( fs::temp_directory_path() / name );
			D_dprintf("tfile path: %s", _full_path.c_str());
		}
		return _full_path;
	}
	inline const std::string& base_name() const { return name; }

	inline const fs::path* operator->() const { return &full_path(); }
private:	
	std::string name;
	mutable fs::path _full_path;
};

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
	file_vector<int> test;
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
