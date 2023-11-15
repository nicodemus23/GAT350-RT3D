#pragma once
#include <string>

namespace nc
{
	std::string ToUpper(const std::string& input);
	std::string ToLower(const std::string& input);
	bool IsEqualIgnoreCase(const std::string& str1, const std::string& str2);
	std::string CreateUnique(const std::string& input);

	// SetBits: sets bits in "a" according to "b" bit pattern 
	// *sets all the bits in "a" that are set in "b"
	template<typename T>
	constexpr T SetBits(T a, T b)
	{
		return a | b;
	}

	// ClearBits: clears bits in "a" that are set in "b"
	// bitwise AND operator 
	template<typename T>
	constexpr T ClearBits(T a, T b) 
	{
		return a & (~b);
	}

	// test if all bits in "a" equal that in "b" 
	// could be a lot of functions 
	// returns true if all the bits in "b" are also set in "a" 
	// returns true if all bits match 
	template<typename T>
	constexpr bool TestBits(T a, T b)
	{
		return (a & b) == b;
	}

	// toggle bits in "a" where bits in "b" are set 
	// "Flip the bits"
	// bitwise exclusive or (XOR)
	template<typename T>
	constexpr T ToggleBits(T a, T b)
	{
		return a ^ b;
	}
}
