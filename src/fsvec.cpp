#include <filesystem>

#include <fsvec.hpp>
#include <uuid.hpp>
#include <fsvec.h>

#define FB file_back_buffer
using std::size_t;
namespace fs = std::filesystem;

/// A temporary file name
struct temp_file
{
	inline temp_file(const temp_file& c) = delete;

	inline temp_file(temp_file&& m) : name(std::move(m.name)) {}
	inline temp_file() : name(uuid::generate().to_string()){}
	inline temp_file(const char* name) : name(name) {}
	inline temp_file(std::string&& name) : name(name) {}

	inline ~temp_file() =  default;
	
	inline const fs::path& full_path() const
	{
		if(_full_path.empty())
			_full_path = fs::canonical( fs::temp_directory_path() / name );
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

	inline ~impl() 
	{
		fvec_close(&backing);
	}
};

FB::FB(size_t cap) : inner(std::make_unique<FB::impl>())
{
	// Set cap
	inner->cap = cap;
	// Create then open `inner->file`
	if(!fvec_new(&inner->backing, inner->file->c_str())) panic("Failed to open backing for temp file buffer");

}
FB::FB() : FB(DEFAULT_CAP){}

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
