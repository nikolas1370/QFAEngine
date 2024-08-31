#pragma once
#include <string>
#include <vector>
#include <Tools/Stuff.h>

class QFAEXPORT QFAString
{
public:
	enum StrValueType
	{
		Percent,
		Pixel
	};

	enum ValueAction
	{
		None = 0, // AnalyzeResult is value
		Add,
		Minus,
		Multiply,
		Division
	};

	struct AnalyzeResult
	{
		ValueAction Action;
		StrValueType Type;
		float Value;
	};

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

	static std::wstring U32stringToWString(std::u32string& str)
	{		
		std::wstring outStr;
		outStr.resize(str.size());
		for (size_t i = 0; i < str.size(); i++)
			outStr[i] = (wchar_t)str[i];

		return outStr;
	}

	static std::string U32stringToString(const std::u32string& str)
	{
		std::string outStr;
		outStr.resize(str.size());
		for (size_t i = 0; i < str.size(); i++)
			outStr[i] = (char)str[i];

		return outStr;
	}

	static std::u32string NumToU32string(size_t num)
	{
		std::string str = std::to_string(num);
		std::u32string bigString;
		bigString.resize(str.size());
		for (size_t i = 0; i < bigString.size(); i++)
			bigString[i] = (char32_t)str[i];

		return bigString;
	}
	/*------------ for engine ------------*/
private:
	static std::vector<AnalyzeResult>& AnalyzeString(const char* str);
public:
	// "17.1% - 5.1 + 2" corect input
	static float GetValue(const char* str, int percentValue, bool is_Size);

};

