#pragma once

#include <cstdint>
#include <cstddef>
#include <memory>
#include <utility>
#include <vector>

#include <panic.h>
#include <debug.h>

#include <shuffle3.h>

template<typename T>
struct i_back_inserter
{
	virtual void push_back(T&& value) =0;
	virtual void pop_back() =0;
	virtual const T& back() const =0;
	virtual T& back() =0;
	virtual const std::size_t size() const =0;

	inline bool is_empty() const { return size()==0; }

	virtual inline ~i_back_inserter() =default;
};

struct i_shunt
{
	virtual bool is_full() const =0;
	virtual inline ~i_shunt() =default;
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
	std::size_t len=0;
	mutable std::vector<unsigned char> current_back; // what an awful hack...
};


template<typename T, std::size_t Spill = FSV_DEFAULT_SPILL_AT >
	requires (Spill > 0)
struct fixed_spill_vector : public i_back_inserter<T>
{
	constexpr const static std::size_t SPILL_AT = Spill;

	inline fixed_spill_vector() : mem(std::make_unique<std::array<T, Spill> >()){
		D_dprintf("alloc cap (static): %lu", Spill);
	}
	inline fixed_spill_vector(const fixed_spill_vector<T>& c) = delete;
	inline fixed_spill_vector(fixed_spill_vector<T>&& m)
		: mem(std::move(m.mem)),
		  mem_fill_ptr(m.mem_fill_ptr),
		  fil(std::move(m.fil))
	{}
	inline ~fixed_spill_vector() = default;

	inline void push_back(T&& value) override
	{
		if(mem_is_full()) { 
			//D_dprintf("Inserting value into fs");
			fil.push_back(std::move(value));
		} else { 
			//D_dprintf("Inserting value into memory");
			(*mem)[++mem_fill_ptr] = value;
		}
	}
	inline void pop_back() override
	{
		if(!size()) return;

		if(fil.size()) {
			//D_dprintf("Popping from fs");
			fil.pop_back();
		} else {
			//D_dprintf("Popping from memory %ld", mem_fill_ptr);
			mem_fill_ptr -= 1;
		}
	}
	inline const T& back() const override
	{	
		if (!size()) panic("back() (const) called on no elements");
		if(fil.size()) return fil.back();
		else return (*mem)[mem_fill_ptr];
	}
	inline T& back() override
	{
		if (!size()) panic("back() called on no elements");
		if(fil.size()) return fil.back();
		else return (*mem)[mem_fill_ptr];
	}
	inline const std::size_t size() const override
	{
		return fil.size() + (std::size_t)(mem_fill_ptr+1);
	}
private:
	inline bool mem_is_full() const { return mem_fill_ptr >= (ssize_t)(Spill-1); }
	ssize_t mem_fill_ptr=-1;

	std::unique_ptr<std::array<T, Spill>> mem;
	file_vector<T> fil;
};

template<typename T, typename Shunt>
	requires(std::is_base_of<i_back_inserter<T>, Shunt >::value)
struct shunt : public i_back_inserter<T>, protected i_shunt
{
	typedef Shunt spill_type;
	
	inline shunt() : shunt(FSV_DEFAULT_SPILL_AT){}
	inline shunt(std::size_t cap) : shunt(cap, cap){}
	inline shunt(std::size_t cap, std::size_t spill) : _spill_at(spill), mem(std::vector<T>()), fil(nullptr) {
		mem.reserve(cap);
		D_dprintf("alloc cap %lu (sz %lu == 0?), spill %lu", cap, mem.size(), spill_at());
	}
	inline shunt(const shunt<T, Shunt>& c) = delete;
	inline shunt(shunt<T, Shunt>&& m) :
		_spill_at(m._spill_at),
		mem(std::move(m.mem)),
		fil(std::move(m.fil)){}

	inline void push_back(T&& value) override
	{
		if(is_full()) {
			spl()->push_back(std::move(value));
		}
		else	mem.push_back(std::move(value));
	}
	inline void pop_back() override
	{
		if(is_full() && spl()->size())  spl()->pop_back();
		else				mem.pop_back();
	}
	inline const T& back() const override
	{
		if(is_full() && spl()->size())  return spl()->back();
		else				return mem.back();
	}
	inline T& back() override
	{
		if(is_full() && spl()->size())  return spl()->back();
		else				return mem.back();	
	}
	inline const std::size_t size() const override { return mem.size() + (fil ? fil->size() : 0); }

	inline const std::size_t spill_at() const { return _spill_at; }
	inline bool is_spilling() const { return is_full(); }
protected:
	inline bool is_full() const override { return size()>=spill_at(); }
private:
	inline const spill_type* spl() const
	{
		if(is_full()) {
			if(!fil) fil = std::make_unique<spill_type>();
			return fil.get();
		} else {
			return fil.get();
		}
	}
	inline spill_type* spl()
	{
 		if(is_full()) {
			if(!fil) fil = std::make_unique<spill_type>();
			return fil.get();
		} else {
			return fil.get();
		}
	}
	std::size_t _spill_at;
	std::vector<T> mem;
	mutable std::unique_ptr<spill_type> fil;
};

template<typename T>
using dynamic_spill_vector = shunt<T, file_vector<T> >;
