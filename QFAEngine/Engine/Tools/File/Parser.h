#pragma once
#include <string>
class Parser // in some time it do real parsing 
{// std::to_string
public:
	/*
		firstChars = pointer in text who start line(0 in end)
		text = text for procesing
		lengthLine = return length line

		return point when line start exclude chareacter in firstChars
			if not find null
	*/
	static const char* GetLine(const char* firstChars, const char* text, int& lengthLine); 
	/*
		name must haw .some
		if after file name stay / or \ nale be calculated like folder
	returl Length Path */
	static std::string GetPathWithoutNameFile(const char* path);

	/*
	* one chars one line 
		const char* arrayChars[] = 
	{
		"Chars1",
		"Chars2"
	};

	int arrayCharsCount[2];


	*/
	static void GetAmountSomeCharsInText(const char* text, int textLen, const char** arrayChars, int* arrayCharsCount, int arrayCharsLen);

	static void GetAmountSomeCharsInText(const std::string text, const char** arrayChars, int* arrayCharsCount, int arrayCharsLen);

	/*
	return pointer first char next line
		when next line does not exist return nullptr
	*/
	static char* SkipLine(const char* text);


	inline static float GetFloatFromString(const char* number)
	{
		char* end;
		return strtof(number, &end);
	}

	inline static unsigned long GetUIntFromString(const char* number)
	{
		char* end;// set ptr after last digit // "123/123" . ptr = "/123"
		return strtoul(number, &end, 10);
		
	}

	/*
	return pointer on next symbol c
		when return nullptr symbol not found
	*/
	static const char* GetNextChar(const char c, const char* text);


	/*
	* lengthChars if -1 end chars be 0
	* 
		return true if symbol in chars equal text
	*/
	static bool CheckChars(const char* chars, const char* text, int lengthChars = -1);

	static int GetLineLenght(const char* text);

	static std::string GetNameFile(const char* path);

	
	
};
