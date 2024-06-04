#pragma once
#include <string>

class QFAString
{
public:
	static float ToFloat(std::u32string string)
	{
		std::string str;
		str.resize(string.size());
		for (size_t i = 0; i < string.size(); i++)
			str[i] = (char)string[i];

		return std::stof(str);
	}

	static std::u32string CharsTo32Chars(const char* str)
	{
		size_t strl = strlen(str);
		std::u32string outStr;
		outStr.resize(strl);
		for (size_t i = 0; i < strl; i++)
			outStr[i] = (char32_t)str[i];

		return outStr;
	}
private:

};

