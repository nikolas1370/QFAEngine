#pragma once
#include <string>
#include <fstream>
#include <sstream>

class FileBinary
{
    
public:
    void* FileBinery;
    bool DeleteFile = true;//if true auto delite in destructor
    size_t Size;
    bool Error;

    FileBinary()
    {
        Error = true;
    }

    FileBinary(void* file, size_t size)
    {
        Error = false;
        FileBinery = file;
        Size = size;
    }

    ~FileBinary()
    {
        if (FileBinery && DeleteFile)
            free(FileBinery);
    }
    
};



class FileLoader
{
public:
    /*
    text file
    */
    static std::string load(std::string path)
    {// 
        std::ifstream stream(path);
        std::string line;
        std::stringstream ss;
        while (getline(stream, line))
            ss << line << '\n';

        return ss.str();
    }

    /*
        return number byte read;
    */
    static int load(std::string path, char* bufferPtr, int bufferSize)
    {
        struct stat buf;
        if (stat(path.c_str(), &buf) == -1)
        {

            std::cout << "FileLoader::load file not found \"" << path << "\"" << std::endl;
            __debugbreak();
            return -1;
        }

        FILE* f = nullptr; // fwrite binery write
        errno_t err = fopen_s(&f, path.c_str(), "rb");
        if (err)
            return 0;


        size_t fileSize = fread(bufferPtr, 1, bufferSize, f);
        fclose(f);
        return (int)fileSize;
    }

    static FileBinary LoadBinary(std::string path)
    {
        struct stat buf;
        stat(path.c_str(), &buf);
        FILE* f = nullptr; // fwrite binery write
        errno_t err = fopen_s(&f, path.c_str(), "rb");
        if (err)
            return FileBinary();

        void* asd = malloc(buf.st_size);
        size_t fileSize = fread(asd, 1, buf.st_size, f);
        
        fclose(f);
        return FileBinary(asd, fileSize);
    }
};

