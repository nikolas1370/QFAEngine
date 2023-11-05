#pragma once
//#include "Math/Vector.h"
//#include "Math/Vector2D.h"
#include <Object/ActorComponent/SceneComponent/Mesh/MeshBase.h> // remove to this file



class QFARender;


class QStaticMesh :  public QMeshBaseComponent
{
    friend QFARender;
    MeshFrames* MFs;
    //friend Render;
    static const int MaxTextureSlot = 32;
    //Texture* Textures[MaxTextureSlot] = {0};
    static const int TexureIndex[]; /* pixel shader now haw order texture in  uniform sampler2D u_Texture // without it pixel shader not know correct order texture slot*/
    /*
    int texureIndex[QStaticMesh::MaxTextureSlot];/* pixel shader now haw order texture in  uniform sampler2D u_Texture*//*
    for (int i = 0; i < QStaticMesh::MaxTextureSlot; i++)// without it pixel shader not now corect texture slot
        texIndex[i] = i;*/

    /*  now be in MeshFrames
    VertexMaterial* Vertexs;
    int VertexesBytes;
    unsigned int* Indices;
    int IndicesCount;
    */



    static QFAShaderProgram* SShaderProgram;
    static QFAShaderProgram* SShaderShadowProgram;

    static std::string VertexShaderPath;
    static std::string FragmentShaderPath;

    static std::string VertexShadowShaderPath;
    static std::string FragmentShadowShaderPath;
        // SShaderProgram = new ShaderProgram(VertexShaderPath, FragmentShaderPath);

protected:
    inline QFAShaderProgram* GetShaderProgram() override
    {
        return SShaderProgram;
    }

    inline QFAShaderProgram* GetShadowShaderProgram() override
    {
        return SShaderShadowProgram;
    }
public:

    QStaticMesh(){}
    
    /*
    vertexSize   == vertex array size in byte
    indicesCount  == index count
    */
    /*
    QStaticMesh(VertexMaterial* vertexs, int vertexSize, unsigned int* indices, int indicesCount);
    QStaticMesh(void* meshInfo, int vertexCount, int indicesCount);
	void SetMesh(VertexMaterial* vertexs, int vertexBufferSize, unsigned int* indices, int indicesCount);
    */
    QStaticMesh(MeshFrames* meshFrames);
    
    void SetMesh(MeshFrames* meshFrames);

    //void SetTexture(int index, Texture* texture);

    void Bind(bool isShadow = false) override;

    int GetIndexCount() override;

    // write not fine
    QStaticMesh* GetCopyMesh();
private:
    
};


