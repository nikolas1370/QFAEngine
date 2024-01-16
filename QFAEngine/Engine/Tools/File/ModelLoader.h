#pragma once
#include <assimp/Importer.hpp>      
#include <assimp/scene.h>           
#include <assimp/postprocess.h>     
//#include <assimp/DefaultLogger.hpp>
#include <assimp/material.h>
#include <Object/ActorComponent/SceneComponent/Mesh/MeshBase.h>
#include <Object/ActorComponent/SceneComponent/Mesh/StaticMesh.h>

#pragma comment(lib, "assimp-vc143-mt.lib")

class QFAModelLoader
{    
    static void CalculateIndex(const aiScene* scene, const aiNode* node, unsigned int& indexCount, unsigned int& vertexCount);

    /*
    * each aiMesh have own index
    * unicueIndex convert aiMesh index in MeshData index
    */
    static void WriteIndex(const aiScene* scene, const aiNode* node, unsigned int* meshIndexes, unsigned int& unicueIndex, unsigned int& indexCount);
    static void WriteVertex(const aiScene* scene, const aiNode* node, unsigned int& verticeCound, SSVertexMaterial* meshVertex, bool isObj);
    static void WriteMaterial(const aiScene* scene, const aiNode* node, MeshData* mesDataReal);
    static aiMatrix4x4 GetFiniteMatrix(const aiNode* node);
public:
	QFAModelLoader();
	~QFAModelLoader();

    /*
        import up Y and forwart -Z because blender defauld setings
    */
    static QStaticMesh* LoadModel(const std::string& pFile);
private:

};

void QFAModelLoader::CalculateIndex(const aiScene* scene, const aiNode* node, unsigned int& indexCount, unsigned int& vertexCount)
{
    for (size_t i = 0; i < node->mNumMeshes; i++)
    {
        unsigned int meshIndex = node->mMeshes[i];
        for (size_t j = 0; j < scene->mMeshes[meshIndex]->mNumFaces; j++)
            indexCount += scene->mMeshes[meshIndex]->mFaces[j].mNumIndices;

        vertexCount += scene->mMeshes[meshIndex]->mNumVertices;
    }
    
    for (size_t i = 0; i < node->mNumChildren; i++)
        CalculateIndex(scene, node->mChildren[i], indexCount, vertexCount);
}

inline void QFAModelLoader::WriteIndex(const aiScene* scene, const aiNode* node, unsigned int* meshIndexes, unsigned int& unicueIndex, unsigned int& indexCount)
{ 
    for (size_t i = 0; i < node->mNumMeshes; i++)
    {
        unsigned int meshIndex = node->mMeshes[i];
        for (size_t j = 0; j < scene->mMeshes[meshIndex]->mNumFaces; j++)
            for (size_t k = 0; k < scene->mMeshes[meshIndex]->mFaces[j].mNumIndices; k++)
                meshIndexes[indexCount++] = scene->mMeshes[meshIndex]->mFaces[j].mIndices[k] + unicueIndex;
        
        unicueIndex += scene->mMeshes[meshIndex]->mNumVertices;
    }

    for (size_t i = 0; i < node->mNumChildren; i++)
        WriteIndex(scene, node->mChildren[i], meshIndexes, unicueIndex, indexCount);
}

inline void QFAModelLoader::WriteVertex(const aiScene* scene, const aiNode* node, unsigned int& verticeCound, SSVertexMaterial* meshVertex, bool isObj)
{
    if(node->mNumMeshes > 0)
    {
        aiMatrix4x4 finiteMatrix;        
        aiVector3D rotationAngel;
        FVector rotations;
        if(!isObj)
        {
            aiVector3D scale;
            aiVector3D rotation;
            aiVector3D position;
            finiteMatrix = GetFiniteMatrix(node);            
            finiteMatrix.Decompose(scale, rotation, position);
            rotations = FVector(Math::RadiansToDegrees(rotation.x), Math::RadiansToDegrees(rotation.y), Math::RadiansToDegrees(rotation.z));            
        }

        for (size_t i = 0; i < node->mNumMeshes; i++)
        {
            unsigned int meshIndex = node->mMeshes[i];
            for (size_t j = 0; j < scene->mMeshes[meshIndex]->mNumVertices; j++)
            {
                if (isObj)
                {
                    aiVector3D vert = scene->mMeshes[meshIndex]->mVertices[j] * 100.0f;
                    meshVertex[verticeCound].Position = FVector(vert.x, vert.y, vert.z);
                    meshVertex[verticeCound].Normal.X = scene->mMeshes[meshIndex]->mNormals[j].x;
                    meshVertex[verticeCound].Normal.Y = scene->mMeshes[meshIndex]->mNormals[j].y;
                    meshVertex[verticeCound].Normal.Z = scene->mMeshes[meshIndex]->mNormals[j].z;
                }
                else// fbx
                {                     
                    aiVector3D vert = finiteMatrix * scene->mMeshes[meshIndex]->mVertices[j];
                    meshVertex[verticeCound].Position = FVector(vert.x, vert.y, vert.z);
                    meshVertex[verticeCound].Normal = FVector(scene->mMeshes[meshIndex]->mNormals[j].x, scene->mMeshes[meshIndex]->mNormals[j].y, scene->mMeshes[meshIndex]->mNormals[j].z)
                        .RotateAngleAxis(rotations.X, FVector(1, 0, 0));
                    meshVertex[verticeCound].Normal = meshVertex[verticeCound].Normal.RotateAngleAxis(rotations.Y, FVector(0, 1, 0));
                    meshVertex[verticeCound].Normal = meshVertex[verticeCound].Normal.RotateAngleAxis(rotations.Z, FVector(0, 0, 1));
                }

                meshVertex[verticeCound].materialIndex = scene->mMeshes[meshIndex]->mMaterialIndex;
                verticeCound++;
            }
        }
    }

    for (size_t i = 0; i < node->mNumChildren; i++)
        WriteVertex(scene, node->mChildren[i],  verticeCound, meshVertex, isObj);
}

inline void QFAModelLoader::WriteMaterial(const aiScene* scene, const aiNode* node, MeshData* mesDataReal)
{
    aiColor3D color(0.f, 0.f, 0.f);
    for (size_t i = 0; i < node->mNumMeshes; i++)
    {
        unsigned int meshIndex = node->mMeshes[i];
        unsigned int materialIndex = scene->mMeshes[meshIndex]->mMaterialIndex;

        scene->mMaterials[materialIndex]->Get(AI_MATKEY_COLOR_DIFFUSE, color);
        Material mat = { FVector(color.r, color.g, color.b), 1 };
        mesDataReal->SetMaterial(mat, materialIndex);
    }

    for (size_t i = 0; i < node->mNumChildren; i++)
        WriteMaterial(scene, node->mChildren[i],  mesDataReal);
}

inline aiMatrix4x4 QFAModelLoader::GetFiniteMatrix(const aiNode* node)
{
    aiMatrix4x4 res = node->mParent->mTransformation * node->mTransformation;
    aiNode* tem = node->mParent;
    while (true)
    {
        if (!tem->mParent)
            break;

        res = tem->mParent->mTransformation * res;
        tem = tem->mParent;
    }

    return res;
}

QFAModelLoader::QFAModelLoader()
{
}

QFAModelLoader::~QFAModelLoader()
{
}

QStaticMesh* QFAModelLoader::LoadModel(const std::string& pFile)
{
    Assimp::Importer importer;

    /*
    Assimp::DefaultLogger::create("", Assimp::Logger::VERBOSE);
    Assimp::LogStream* stderrStream = Assimp::LogStream::createDefaultStream(aiDefaultLogStream_STDERR);
    Assimp::DefaultLogger::get()->attachStream(stderrStream, Assimp::Logger::NORMAL | Assimp::Logger::DEBUGGING | Assimp::Logger::VERBOSE);
    */
    const aiScene* scene = importer.ReadFile(pFile,
        aiProcess_CalcTangentSpace |
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType |
        aiProcess_FlipWindingOrder |
        aiProcess_GenSmoothNormals 
    ); /*//aiProcess_GenNormals // */

    if (nullptr == scene)
    {
        std::cout << "fail\n";
        return nullptr;
    }
    
    bool obj = false;

    aiString objFormat("Wavefront Object Importer");
    aiString fbxFormat("Autodesk FBX Importer");
    aiString str("SourceAsset_Format");

    for (size_t i = 0; i < scene->mMetaData->mNumProperties; i++)
    {
        aiMetadataEntry* kio = &scene->mMetaData->mValues[i];
        if ((str == scene->mMetaData->mKeys[i]))
        {
            if (*((aiString*)kio->mData) == objFormat)
                obj = true;
            else if (*((aiString*)kio->mData) == fbxFormat)
                obj = false;

            break;
        }
    }

    unsigned int allIndexCound = 0;
    unsigned int verticeCound = 0;
    for (size_t i = 0; i < scene->mRootNode->mNumChildren; i++)
        CalculateIndex(scene, scene->mRootNode->mChildren[i], allIndexCound, verticeCound);


    std::cout << allIndexCound << " " << verticeCound << " lopoid\n";

    MeshData* mesDataReal = new MeshData(verticeCound, allIndexCound, scene->mNumMaterials, 0);
    
    //----------
    unsigned int* meshIndexes = mesDataReal->GetIndexData();;

    allIndexCound = 0;

    unsigned int unicueIndex = 0;
    for (size_t i = 0; i < scene->mRootNode->mNumChildren; i++)
        WriteIndex(scene, scene->mRootNode->mChildren[i], meshIndexes, unicueIndex, allIndexCound);

    SSVertexMaterial* meshVertex = mesDataReal->GetFrameData(0);
    verticeCound = 0;
    
    for (size_t i = 0; i < scene->mRootNode->mNumChildren; i++)
        WriteVertex(scene, scene->mRootNode->mChildren[i], verticeCound, meshVertex, obj);    

    for (size_t i = 0; i < scene->mRootNode->mNumChildren; i++)
        WriteMaterial(scene, scene->mRootNode->mChildren[i], mesDataReal);  

    QStaticMesh* meshReal = new QStaticMesh(mesDataReal);

    return meshReal;
}
