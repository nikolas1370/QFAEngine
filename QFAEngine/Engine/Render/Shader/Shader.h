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

class Shader
{
    int init = false;

    unsigned int ShaderID;
    std::string readShader(const std::string path);
public:
    Shader();

	Shader(ShaderType st, const std::string& shader, bool isPath = true);
    unsigned int GetId() const;
    
    unsigned int CreateShader(ShaderType st, const std::string& shader, bool isPath = true);
};

