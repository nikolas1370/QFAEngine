#include "Parser.h"

#include <iostream>

const char* Parser::GetLine(const char* firstChars, const char* text, int& lengthLine)
{
   // std::cout << firstChars << std::endl;
    lengthLine = 0;
    bool skipLine = false;
    bool find = false;
    int lineStart = -1;
    for (int i = 0; ; i++)
    {   
        if (find)
        {            
            if (text[i] == 0 || text[i] == '\n')
                break;
            else if (lineStart == -1)
                lineStart = i;
            
            lengthLine++;
            continue;
        }
        else if (text[i] == 0)
            return nullptr;

        if (skipLine && !find)
        {
            if (text[i] == '\n')
                skipLine = false;

            continue;
        }
        else if (text[i] == '\n')
            continue;

        for (int j = 0;; j++)
        {
            if (firstChars[j] == 0)
            {
                i--;
                find = true;
                break;
            }
            else if (firstChars[j] != text[i])
                break;
            
            i++;                
        }        

        skipLine = true;
    }
    
    return &text[lineStart];
}

std::string Parser::GetPathWithoutNameFile(const char* path)
{
    int len = 0;
    int canBeFileNameCount = 0;
    for (int i = 0;; i++)
    {
        if (path[i] == 0)
            break;

        canBeFileNameCount++;
        if (path[i] == '\\' || path[i] == '/')
        {
            len += canBeFileNameCount;
            canBeFileNameCount = 0;
            continue;
        }
    }
    
    return std::string(path, len);
}

std::string Parser::GetNameFile(const char* path)
{
    int nameStart = 0;
    for (int i = 0; ; i++)
    {
        if (path[i] == 0)
            return std::string(&path[nameStart]);

        if (path[i] == '\\' || path[i] == '/')
            nameStart = i + 1;
    }

    return std::string("");
}




void Parser::GetAmountSomeCharsInText(const std::string text, const char** arrayChars, int* arrayCharsCount, int arrayCharsLen)
{
    bool skipLine = false;
    for (int i = 0; i < text.length(); i++)
    {
        if (skipLine)
        {
            if (text[i] == '\n')
                skipLine = false;

            continue;
        }

        for (int j = 0; j < arrayCharsLen; j++)
        {
            bool find = false;
            for (int newI = i, k = 0;; newI++, k++)
            {
                if (text[newI] == 0)
                    break;
                else if (arrayChars[j][k] == 0)
                {
                    arrayCharsCount[j]++;
                    find = true;
                    break;
                }
                else if (text[newI] == arrayChars[j][k])
                    continue;
                else
                    break;
            }

            if (find)
                break;
        }

        skipLine = true;
    }
}

void Parser::GetAmountSomeCharsInText(const char* text, int textLen, const char** arrayChars, int* arrayCharsCount, int arrayCharsLen)
{
    bool skipLine = false;
    for (int i = 0; i < textLen; i++)
    {
        if (skipLine)
        {
            if(text[i] == '\n')
                skipLine = false;
            
            continue;
        }

        for (int j = 0; j < arrayCharsLen; j++)
        {            
            bool find = false;
            for(int newI = i, k = 0;; newI++, k++)
            {
                if (text[newI] == 0)
                    break;
                else if (arrayChars[j][k] == 0)
                {
                    arrayCharsCount[j]++;
                    find = true;
                    break;
                }
                else if (text[newI] == arrayChars[j][k])
                    continue;
                else
                    break;
            }

            if (find)
                break;
        }

        skipLine = true;
    }    
}

char* Parser::SkipLine(const char* text)
{
    if (!text)
        return nullptr;

    for (int i = 0;; i++)
    {
        if (text[i] == 0)
            return nullptr;
        else if (text[i] == '\n')
            return (char*)&text[i + 1];
    }

    return nullptr;
}

const char* Parser::GetNextChar(const char c, const char* text)
{
    for (int i = 0;; i++)
    {
        if (text[i] == 0)
            return nullptr;
        else if (text[i] == c)
            return &text[i];
    }
}

bool Parser::CheckChars(const char* chars, const char* text, int lengthChars)
{        
    for (size_t i = 0; i < lengthChars || lengthChars < 0; i++)
    {        
        if (chars[i] == 0)
            return true;
        else if (chars[i] != text[i])
            return false;
    }

    return true;
}

int Parser::GetLineLenght(const char* text)
{
    int len = 0;
    for (int i = 0; ; i++)
    {        
        if (text[i] == 0 || text[i] == '\n')
            return len;
        else
            len++;
    }
}



