
#include "Shader.h"
#include <Tools/Debug/OpenGlStuff.h>


std::string QFAShader::readShader(const std::string path)
{
    std::ifstream stream(path);
    std::string line;
    std::stringstream ss;
    while (getline(stream, line))
        ss << line << '\n';

    return ss.str();
}

QFAShader::QFAShader()
{
}

QFAShader::QFAShader(ShaderType st, const std::string& shader, bool isPath)
{
    CreateShader(st, shader, isPath);
}

unsigned int QFAShader::GetId() const
{
    return ShaderID;
}

unsigned int QFAShader::CreateShader(ShaderType st, const std::string& shader, bool isPath)
{
    if (init)
        return ShaderID;    

    std::string shader_;
    if (isPath)
        shader_ = readShader(shader);
    else
        shader_ = shader;

    if (shader_.length() == 0)
    {
        std::cout << "[compile shader error] (file path=./" << shader << " )" << " Path Error. File does not exist or file length equal zero"<< std::endl;
        ASSERT(false);
    }

    GLCall(ShaderID = glCreateShader((st == ShaderType::STVertex ? GL_VERTEX_SHADER : (st == ShaderType::STFragment ? GL_FRAGMENT_SHADER : GL_GEOMETRY_SHADER)))); // create void shader
    const char* src = shader_.c_str();
    GLCall(glShaderSource(ShaderID, 1, &src, nullptr)); // add shader source in shader
    GLCall(glCompileShader(ShaderID));

    int result;
    glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &result);// get resoult compilation
    if (result == GL_FALSE)
    {
        int len;
        glGetShaderiv(ShaderID, GL_INFO_LOG_LENGTH, &len);
        char* message = (char*)alloca(len);// Функция alloca() выделяет size байт памяти из стека системы (не из кучи) и возвращает на него указатель. Если запрос на выделение памяти не выполнен, то возвращается нулевой указатель.
        glGetShaderInfoLog(ShaderID, len, &len, message);
        std::cout << "[compile shader error] (file path=./" << shader << " ) " << message << std::endl;
        glDeleteShader(ShaderID);
        
        ASSERT(false);
    }

    init = true;
    return ShaderID;
}
