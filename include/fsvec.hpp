#pragma once

#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

#include <panic.h>

struct file_back_buffer
{
	const static constexpr std::size_t DEFAULT_CAP = 1024;
	typedef std::uint8_t byte;

	file_back_buffer();
	file_back_buffer(std::size_t cap);
	file_back_buffer(const file_back_buffer& c) = delete;
	inline file_back_buffer(file_back_buffer&& m) : inner(std::move(m.inner)){}

	void push_buf(byte* buf, std::size_t len);
	bool back(byte* buf, std::size_t len) const;
	bool pop_n(std::size_t len);

	~file_back_buffer();
private:
	struct impl;
	std::unique_ptr<impl> inner;
};

namespace {
	template<typename T>
	inline const T* _die_if_null(const T* input, const char* msg)
	{
		if(!input) panic(msg);
		return input;
	}
	template<typename T>
	inline T* _die_if_null(T* input, const char* msg)
	{
		if(!input) panic(msg);
		return input;
	}
}

template<typename T>
struct file_vector
{
	inline file_vector() : file_vector(file_back_buffer::DEFAULT_CAP){}
	inline file_vector(std::size_t cap) : inserter(file_back_buffer(cap)), len(0), current_back(std::vector(sizeof(T))) {current_back.resize(sizeof(T));}
	inline file_vector(const file_vector<T>& c) = delete;
	inline file_vector(file_vector<T>&& m) : inserter(std::move(m.inserter)), len(m.len), current_back(std::move(m.current_back)){}

	inline void push_back(T&& value)
	{
		inserter.push_buf((file_back_buffer::byte*)&value, sizeof(T));
		len += 1;
	}
	inline T& back()
	{
		if(!len) panic("back() called on empty file_vector");
		if(!inserter.back(&current_back[0], sizeof(T))) panic("back() failed");
		return *_die_if_null((T*)&current_back[0], "file_vector::back() returned null pointer");
	}
	inline const T& back() const
	{
		if(!len) panic("back() called on empty file_vector");
		if(!inserter.back(&current_back[0], sizeof(T))) panic("back() failed");
		return *_die_if_null((const T*)&current_back[0], "file_vector::back() (const) returned null pointer");
	}
	inline void pop_back()
	{
		if(!len) return;

		if(!inserter.pop_n(sizeof(T))) panic("pop_back(): 0 elements");
		len-=1;
	}
private:
	mutable std::vector<unsigned char> current_back; // what an awful hack...
	file_back_buffer inserter;
	std::uint64_t len;
};
