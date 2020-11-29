#include <fsvec.hpp>

#define FB file_back_buffer
using std::size_t;

struct temp_file
{
	inline temp_file()
	{
		// Create random new file
	}
	inline temp_file(const temp_file& c) = delete;
	inline temp_file(temp_file&& m) : name(m.name) { m.name= nullptr; }
	inline temp_file(const char* name) : name(name) {}
	inline ~temp_file()
	{
		if(name) name = nullptr;
	}

private:
	const char* name;
};

struct FB::impl
{
	size_t cap;
	temp_file file;
};

FB::FB(size_t cap) : inner(std::make_unique<FB::impl>())
{
	// Set cap
	inner->cap = cap;
	// Open `inner->file`

}
FB::FB() : FB(DEFAULT_CAP){}

void FB::push_buf(byte* buf, size_t len)
{

}

FB::byte* FB::back(size_t len)
{
	return nullptr;
}

const FB::byte* FB::back(size_t len) const
{
	return nullptr;
}

void FB::pop_n(size_t len)
{
	
}
