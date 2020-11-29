#pragma once

#include <random>
#include <string>

struct uuid //not to spec but idc
{
	const static constexpr int SIZE=16;
	inline static uuid generate()
	{
		using namespace std;
		static thread_local random_device dev;
		static thread_local mt19937 rng(dev());
		uniform_int_distribution<int> dist(0, 15);
		constexpr const char hex[] = "0123456789abcdef";
		uuid id;
		for(int i=0;i<SIZE;i++) id.str[i] = hex[dist(rng)];
		id.str[SIZE] =0;
		return id;
	}
	inline operator const char*() const { return &str[0]; }
	inline operator const std::array<char, SIZE+1>&() const { return str; }

	inline std::string to_string() const
	{
		return std::string(&str[0]);
	}
private:
	inline uuid(){}
	std::array<char, SIZE+1> str;
};
