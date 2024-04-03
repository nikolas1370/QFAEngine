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
private:

};

