#include "StringUtils.h"
#include <algorithm>

namespace nc
{	// retains its value across iterations and only accessible inside function (static) // non-negative integer // 
	static unsigned int unique; 

	std::string StringUtils::ToUpper(const std::string& input)
	{	// new string is created as a copy of input 
		std::string str = input;
		// transform function applies toupper function to each character in string 
		std::transform(str.begin(), str.end(), str.begin(), ::toupper); // beginning of string, end of string, str storage location in upper case 


		return str;
	}

	std::string StringUtils::ToLower(const std::string& input)
	{   // new string is created as a copy of input 
		std::string str = input;
		// transform function applies tolower function to each character in string 
		std::transform(str.begin(), str.end(), str.begin(), ::tolower); // beginning of string, end of string, str storage location in lower case

		return str;
	}
	// boolean operator that returns true or false based on the conditions laid out below 
	bool StringUtils::IsEqualIgnoreCase(const std::string& str1, const std::string& str2)
	{
		{  // if string 1 is not equal to string 2, return false ...
			if (str1.size() != str2.size())
				return false;
		} // ... otherwise, std::equal compares each char of two strings (ignoring case) - lambda function converts each char to lower case and compares (which ignores the case) // [capture clause] - none
		return std::equal(str1.begin(), str1.end(), str2.begin(), [](char a, char b) // beginning of string, end of string, str storage location in upper case
			{	// body of lambda function - converts to lower and returns boolean
				return tolower(a) == tolower(b);
			});
	}
	// appends a unique number to an input string and returns the str
	std::string StringUtils::CreateUnique(const std::string& input)
	{	// in increments n_counter, converts to a string and appends to input string each time function is called 
		return input + std::to_string(unique++);
	}



}