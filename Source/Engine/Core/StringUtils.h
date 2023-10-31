#pragma once
#include <string>

namespace nc
{
	class StringUtils
	{
	public:
		// static so they can be called directly on the class without creating an instance (non-member functions)
		static std::string ToUpper(const std::string& input);
		static std::string ToLower(const std::string& input);
		static bool IsEqualIgnoreCase(const std::string& str1, const std::string& str2);
		static std::string CreateUnique(const std::string& input);

	};
}