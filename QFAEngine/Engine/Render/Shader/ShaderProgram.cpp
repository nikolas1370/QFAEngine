#include "ShaderProgram.h"
#include <Tools/Debug/OpenGlStuff.h>


#include <iostream>
#include <Object/ActorComponent/SceneComponent/Mesh/MeshBase.h>// for Material

unsigned int ShaderProgram::CurentRunProgramId = 1234567890;

ShaderProgram::ShaderProgram(std::string vertex, std::string fragment, bool isPath, std::string geometry)
{
    for (int i = 0; i < 101; i++)
        LocationMaterials[i] = -2;

    GLCall(ShaderProgramId = glCreateProgram());
    Shader vs(ShaderType::STVertex, vertex, isPath);
    Shader ps(ShaderType::STFragment, fragment, isPath);    
    Shader gs;
 
    

    


    GLCall(glAttachShader(ShaderProgramId, vs.GetId()));
    GLCall(glAttachShader(ShaderProgramId, ps.GetId()));
    if (geometry != "")
    {

        GLCall(glAttachShader(ShaderProgramId, gs.CreateShader(ShaderType::STGeometry, geometry, isPath)));
    }


    GLCall(glLinkProgram(ShaderProgramId));
    GLCall(glValidateProgram(ShaderProgramId));
    // glValidateProgram learn more  it's interesting


    int program_linked;
    glGetProgramiv(ShaderProgramId, GL_LINK_STATUS, &program_linked);
    if (program_linked != GL_TRUE)
    {
        GLsizei log_length = 0;
        GLchar message[1024];
        glGetProgramInfoLog(ShaderProgramId, 1024, &log_length, message);
        std::cout << "[Link Program(shader) error ] " << message << vertex << "\n\n" << fragment << std::endl;
        while (true)
        {

        }
        ASSERT(false);
    }


}

ShaderProgram::~ShaderProgram()
{
}



void ShaderProgram::SetProjectionMatrix(const glm::mat4& matrix)
{
    // remove
    //Use();
    if (LocationProjectionMatrix == -2)
    {        
        GLCall(LocationProjectionMatrix = glGetUniformLocation(ShaderProgramId, "projection"));
        if (LocationProjectionMatrix == -1)
        {
            std::cout << "ShaderProgram::SetProjectionMatrix in shader problem not found uniform \"projection\"" << std::endl;
            __debugbreak();
            return;
        }
    }

    GLCall(glUniformMatrix4fv(LocationProjectionMatrix, 1, false, &matrix[0][0]))
}

void ShaderProgram::SetCameraRotationMatrix(const glm::mat4& matrix)
{
    if (LocationCameraRotationMatrix == -2)
    {

        GLCall(LocationCameraRotationMatrix = glGetUniformLocation(ShaderProgramId, "cameraR"));
        if (LocationCameraRotationMatrix == -1)
        {
            std::cout << "ShaderProgram::SetCameraRotationMatrix in shader problem not found uniform \"cameraR\"" << std::endl;
            __debugbreak();
            return;
        }        
    }

    GLCall(glUniformMatrix4fv(LocationCameraRotationMatrix, 1, false, &matrix[0][0]));
}

void ShaderProgram::SetCameraPosition(const FVector& position)
{
    if (LocationCameraPosition == -2)
    {
        GLCall(LocationCameraPosition = glGetUniformLocation(ShaderProgramId, "cameraP"));
        if (LocationCameraPosition == -1)
        {
            std::cout << "ShaderProgram::SetCameraPosition in shader problem not found uniform \"cameraP\"" << std::endl;
            __debugbreak();
            return;
        }    
    }

    GLCall(glUniform3f(LocationCameraPosition, position.X, position.Y, position.Z));
}

void ShaderProgram::SetModelMatrix(const glm::mat4& matrix)
{
    if (LocationModelMatrix == -2)
    {
        GLCall(LocationModelMatrix = glGetUniformLocation(ShaderProgramId, "model"));
        if (LocationModelMatrix == -1)
        {
            std::cout << "ShaderProgram::SetModelMatrix in shader problem not found uniform \"model\"" << std::endl;
            __debugbreak();
            return;
        }
    }

    GLCall(glUniformMatrix4fv(LocationModelMatrix, 1, false, &matrix[0][0]));
}

void ShaderProgram::Use()
{
    
    if (ShaderProgramId == CurentRunProgramId)
        return;
    
    
    GLCall( glUseProgram(ShaderProgramId));
    CurentRunProgramId = ShaderProgramId;
}

void ShaderProgram::SetInterpolationTime(const float interpolationTime)
{

    if (LocationInterpolationTime == -2)
    {
        GLCall(LocationInterpolationTime = glGetUniformLocation(ShaderProgramId, "InterTime"));
        if (LocationInterpolationTime == -1)
        {
            std::cout << "ShaderProgram::SetModelMatrix in shader problem not found uniform \"InterTime\"" << std::endl;
            __debugbreak();
            return;
        }
    }

    GLCall(glUniform1f(LocationInterpolationTime, interpolationTime));
}

void ShaderProgram::SetMaterials(Material* materials, int count)
{
    //if (count > 101)  do error
    
   // std::cout << LocationMaterials[0] << LocationMaterials[1]<< LocationMaterials[2] << std::endl;

    for (int i = 0; i < count; i++)
    {
        //std::string uniformName = std::string("materials[1].Color");
        
        //std::cout << "QFA 1 " << LocationMaterials[i] << std::endl;
        if (LocationMaterials[i] == -2)
        {
            std::string uniformName = std::string("materials[").append(std::to_string(i)).append(std::string("].Color"));            
            GLCall( LocationMaterials[i] = glGetUniformLocation(ShaderProgramId, uniformName.c_str()));
          //  std::cout << "QFA 2" << std::endl;
            if (LocationMaterials[i] == -1)
            {                
                std::cout << "ShaderProgram::SetMaterial in shader problem not found uniform \"materials\"" << std::endl;
                __debugbreak();
                return;
            }
        }

        
        //GLCall(LocationMaterials[i] = glGetUniformLocation(ShaderProgramId, uniformName.c_str()));
        //GL_INVALID_OPERATION
        //std::cout << i << " " << materials[i].Color << std::endl;
        GLCall(glUniform3f(LocationMaterials[i], materials[i].Color.X, materials[i].Color.Y, materials[i].Color.Z));
    }
    

}
void ShaderProgram::SetDirectionShadowMap(unsigned int id, int textureSlote)
{// LocationDirectionShadowMap
    if (LocationDirectionShadowMap == -2)
    {
        
        GLCall(LocationDirectionShadowMap = glGetUniformLocation(ShaderProgramId, "shadowMap"));
        //  std::cout << "QFA 2" << std::endl;
        if (LocationDirectionShadowMap == -1)
        {
            std::cout << "ShaderProgram::SetDirectionShadowMap in shader problem not found uniform \"shadowMap\"" << std::endl;
            __debugbreak();
            return;
        }
    }



    GLCall(glActiveTexture(GL_TEXTURE0 + textureSlote));
    GLCall(glBindTexture(GL_TEXTURE_2D, id)); // bind texture to GL_TEXTURE0 + textureSlote
    GLCall(glUniform1i(LocationDirectionShadowMap, textureSlote));
}
void ShaderProgram::SetDirectionLigthMatrix(const glm::mat4& matrix)
{
    // 
    if (LocationDirectionLigthMatrix == -2)
    {

        GLCall(LocationDirectionLigthMatrix = glGetUniformLocation(ShaderProgramId, "directionLightMatrix"));
        //  std::cout << "QFA 2" << std::endl;
        if (LocationDirectionLigthMatrix    == -1)
        {
            std::cout << "ShaderProgram::SetDirectionLigthMatrix in shader problem not found uniform \"directionLightMatrix\"" << std::endl;
            __debugbreak();
            return;
        }
    }

    GLCall(glUniformMatrix4fv(LocationDirectionLigthMatrix, 1, false, &matrix[0][0]));
}
void ShaderProgram::SetShadowOn(bool castShadow)
{
    
    
    if (LocationShadowOn == -2)
    {

        GLCall(LocationShadowOn = glGetUniformLocation(ShaderProgramId, "shadowOn"));
        //  std::cout << "QFA 2" << std::endl;
        if (LocationShadowOn == -1)
        {
            std::cout << "ShaderProgram::SetShadowOn in shader problem not found uniform \"shadowOn\"" << std::endl;
            __debugbreak();
            return;
        }
    }

    GLCall(glUniform1i(LocationShadowOn, (int)castShadow));
}
// 