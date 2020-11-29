#pragma once

#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

#include <panic.h>

template<typename T>
struct i_back_inserter
{
	virtual void push_back(T&& value) =0;
	virtual void pop_back() =0;
	virtual const T& back() const =0;
	virtual T& back() =0;
	virtual const std::size_t size() const =0;

	virtual inline ~i_back_inserter() =default;
};

struct file_back_buffer
{
	const static constexpr std::size_t DEFAULT_CAP = 1024;
	typedef std::uint8_t byte;

	file_back_buffer();
	file_back_buffer(std::size_t cap);
	file_back_buffer(const file_back_buffer& c) = delete;
	file_back_buffer(file_back_buffer&& m);

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
struct file_vector : public i_back_inserter<T>
{
	inline file_vector() : file_vector(file_back_buffer::DEFAULT_CAP){}
	inline file_vector(std::size_t cap) : inserter(file_back_buffer(cap)), len(0), current_back(std::vector<unsigned char>(sizeof(T))) {current_back.resize(sizeof(T));}
	inline file_vector(const file_vector<T>& c) = delete;
	inline file_vector(file_vector<T>&& m) : inserter(std::move(m.inserter)), len(m.len), current_back(std::move(m.current_back)){}

	inline void push_back(T&& value) override
	{
		inserter.push_buf((file_back_buffer::byte*)&value, sizeof(T));
		len += 1;
	}
	inline T& back() override
	{
		if(!len) panic("back() called on empty file_vector");
		if(!inserter.back(&current_back[0], sizeof(T))) panic("back() failed");
		return *_die_if_null((T*)&current_back[0], "file_vector::back() returned null pointer");
	}
	inline const T& back() const override
	{
		if(!len) panic("back() called on empty file_vector");
		if(!inserter.back(&current_back[0], sizeof(T))) panic("back() failed");
		return *_die_if_null((const T*)&current_back[0], "file_vector::back() (const) returned null pointer");
	}
	inline void pop_back() override
	{
		if(!len) return;

		if(!inserter.pop_n(sizeof(T))) panic("pop_back(): 0 elements");
		len-=1;
	}

	inline const std::size_t size() const override { return len; }
private:
	file_back_buffer inserter;
	std::size_t len;
	mutable std::vector<unsigned char> current_back; // what an awful hack...
};

#define DEFAULT_SPILL_AT (1024 * 1024)
template<typename T, std::size_t Spill = DEFAULT_SPILL_AT >
	requires (Spill > 0)
struct fixed_spill_vector : public i_back_inserter<T>
{
	constexpr const static std::size_t SPILL_AT = Spill;

	inline fixed_spill_vector(){}
	inline fixed_spill_vector(const fixed_spill_vector<T>& c) = delete;
	inline fixed_spill_vector(fixed_spill_vector<T>&& m)
		: mem(std::move(m.mem)),
		  mem_fill_ptr(m.mem_fill_ptr),
		  fil(std::move(m.fil))
	{}

	//TODO: Inserters/getters
	inline void push_back(T&& value) override
	{

	}
	inline void pop_back() override
	{

	}
	inline const T& back() const override
	{

	}
	inline T& back() override
	{

	}
	inline const std::size_t size() const override
	{

	}
private:
	std::array<T, Spill> mem;
	std::size_t mem_fill_ptr;

	file_vector<T> fil;
};
