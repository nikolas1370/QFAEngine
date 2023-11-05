#pragma once
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <string>

enum ShaderType
{
    STVertex,
    STFragment,
    STGeometry

};

class QFAShader
{
    int init = false;

    unsigned int ShaderID;
    std::string readShader(const std::string path);
public:
    QFAShader();

    QFAShader(ShaderType st, const std::string& shader, bool isPath = true);
    unsigned int GetId() const;
    
    unsigned int CreateShader(ShaderType st, const std::string& shader, bool isPath = true);
};

