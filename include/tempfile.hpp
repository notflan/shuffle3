#pragma once

#include <filesystem>
#include <string>
#include <utility>
#include "uuid.hpp"

#include <debug.h>

using std::size_t;
namespace fs = std::filesystem;

/// A temporary file name
struct temp_file
{
	inline temp_file(const temp_file& c) = delete;

	inline temp_file(temp_file&& m) : name(std::move(m.name)), _full_path(std::move(m._full_path)) {}
	inline temp_file() : name(uuid::generate().to_string()+"-s3"){}
	inline temp_file(const char* name) : name(name) {}
	inline temp_file(std::string&& name) : name(name) {}

	inline ~temp_file()
	{
		if(name.empty() && _full_path.empty()) return;

		D_dprintf("~tempfile(): %s", _full_path.c_str());

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
