#pragma once
#include <assimp/Importer.hpp>      
#include <assimp/scene.h>           
#include <assimp/postprocess.h>     
//#include <assimp/DefaultLogger.hpp>
#include <assimp/material.h>
#include <Object/ActorComponent/SceneComponent/Mesh/MeshBase.h>
#include <Object/ActorComponent/SceneComponent/Mesh/StaticMesh.h>
#include <Tools/Debug/OpenGlStuff.h>

#pragma comment(lib, "assimp-vc143-mt.lib")

class QFAModelLoader
{    

    enum EFileFormat : char8_t
    {
        UNDEFINED,
        OBJ,
        FBX
    };

    static EFileFormat GetFileFormat(const aiScene* scene);

    static void CalculateIndex(const aiScene* scene, const aiNode* node, unsigned int& indexCount, unsigned int& vertexCount, bool calculateChildren);

    /*
    * each aiMesh have own index
    * unicueIndex convert aiMesh index in MeshData index
    */
    static void WriteIndex(const aiScene* scene, const aiNode* node, unsigned int* meshIndexes, unsigned int& unicueIndex, unsigned int& indexCount, bool processChildren);
    static void WriteVertex(const aiScene* scene, const aiNode* node, unsigned int& verticeCound, SSVertexMaterial* meshVertex, EFileFormat fileFormat, bool processChildren);
    static void WriteMaterial(const aiScene* scene, const aiNode* node, MeshData* mesDataReal, bool processChildren);
    static aiMatrix4x4 GetFiniteMatrix(const aiNode* node);
public:
	QFAModelLoader();
	~QFAModelLoader();

    /*
        import up Y and forwart -Z because blender defauld setings
    */
    static QStaticMesh* LoadModel(const std::string& pFile);
    static std::vector<QStaticMesh*> LoadModelSeparate(const std::string& pFile);
    //load model separate
private:
    static void GetMesh(const aiScene* scene, const aiNode* node, EFileFormat fileFormat, std::vector<QStaticMesh*>& vector);

};

inline QFAModelLoader::EFileFormat QFAModelLoader::GetFileFormat(const aiScene* scene)
{
    aiString objFormat("Wavefront Object Importer"); 
    aiString fbxFormat("Autodesk FBX Importer");
    aiString str("SourceAsset_Format");
    for (size_t i = 0; i < scene->mMetaData->mNumProperties; i++)
    {
        aiMetadataEntry* kio = &scene->mMetaData->mValues[i];
        if ((str == scene->mMetaData->mKeys[i]))
        {
            if (*((aiString*)kio->mData) == objFormat)
                return EFileFormat::OBJ;
            else if (*((aiString*)kio->mData) == fbxFormat)
                return EFileFormat::FBX;

            break;
        }
    }
    
    return EFileFormat::UNDEFINED;
}

void QFAModelLoader::CalculateIndex(const aiScene* scene, const aiNode* node, unsigned int& indexCount, unsigned int& vertexCount, bool calculateChildren)
{
    for (size_t i = 0; i < node->mNumMeshes; i++)
    {
        unsigned int meshIndex = node->mMeshes[i];
        for (size_t j = 0; j < scene->mMeshes[meshIndex]->mNumFaces; j++)
            indexCount += scene->mMeshes[meshIndex]->mFaces[j].mNumIndices;

        vertexCount += scene->mMeshes[meshIndex]->mNumVertices;
    }
    
    if(calculateChildren)
        for (size_t i = 0; i < node->mNumChildren; i++)
            CalculateIndex(scene, node->mChildren[i], indexCount, vertexCount, calculateChildren);
}

inline void QFAModelLoader::WriteIndex(const aiScene* scene, const aiNode* node, unsigned int* meshIndexes, unsigned int& unicueIndex, unsigned int& indexCount, bool processChildren)
{ 
    for (size_t i = 0; i < node->mNumMeshes; i++)
    {
        unsigned int meshIndex = node->mMeshes[i];
        for (size_t j = 0; j < scene->mMeshes[meshIndex]->mNumFaces; j++)
            for (size_t k = 0; k < scene->mMeshes[meshIndex]->mFaces[j].mNumIndices; k++)
                meshIndexes[indexCount++] = scene->mMeshes[meshIndex]->mFaces[j].mIndices[k] + unicueIndex;
        
        unicueIndex += scene->mMeshes[meshIndex]->mNumVertices;
    }
    if(processChildren)
        for (size_t i = 0; i < node->mNumChildren; i++)
            WriteIndex(scene, node->mChildren[i], meshIndexes, unicueIndex, indexCount, processChildren);
}

inline void QFAModelLoader::WriteVertex(const aiScene* scene, const aiNode* node, unsigned int& verticeCound, SSVertexMaterial* meshVertex, EFileFormat fileFormat, bool processChildren)
{
    if(node->mNumMeshes > 0)
    {
        aiMatrix4x4 finiteMatrix;        
        aiVector3D rotationAngel;
        FVector rotations;
        if(fileFormat == EFileFormat::FBX)
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
                if (fileFormat == EFileFormat::OBJ || fileFormat == EFileFormat::UNDEFINED)
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

    if(processChildren)
        for (size_t i = 0; i < node->mNumChildren; i++)
            WriteVertex(scene, node->mChildren[i],  verticeCound, meshVertex, fileFormat, processChildren);
}

inline void QFAModelLoader::WriteMaterial(const aiScene* scene, const aiNode* node, MeshData* mesDataReal, bool processChildren)
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

    if(processChildren)
        for (size_t i = 0; i < node->mNumChildren; i++)
            WriteMaterial(scene, node->mChildren[i],  mesDataReal, processChildren);
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
    ); 

    if (nullptr == scene)
    {
        std::cout << "fail\n";
        ASSERT(false);
        return nullptr;
    }
   
    EFileFormat fileFormat = GetFileFormat(scene);  
    unsigned int allIndexCound = 0;
    unsigned int verticeCound = 0;
    for (size_t i = 0; i < scene->mRootNode->mNumChildren; i++)
        CalculateIndex(scene, scene->mRootNode->mChildren[i], allIndexCound, verticeCound, true);

    MeshData* mesDataReal = new MeshData(verticeCound, allIndexCound, scene->mNumMaterials, 0);

    unsigned int* meshIndexes = mesDataReal->GetIndexData();

    allIndexCound = 0;

    unsigned int unicueIndex = 0;
    for (size_t i = 0; i < scene->mRootNode->mNumChildren; i++)
        WriteIndex(scene, scene->mRootNode->mChildren[i], meshIndexes, unicueIndex, allIndexCound, true);

    SSVertexMaterial* meshVertex = mesDataReal->GetFrameData(0);
    verticeCound = 0;
    
    for (size_t i = 0; i < scene->mRootNode->mNumChildren; i++)
        WriteVertex(scene, scene->mRootNode->mChildren[i], verticeCound, meshVertex, fileFormat, true);

    for (size_t i = 0; i < scene->mRootNode->mNumChildren; i++)
        WriteMaterial(scene, scene->mRootNode->mChildren[i], mesDataReal, true);
    
    importer.FreeScene();
    return new QStaticMesh(mesDataReal);
}

std::vector<QStaticMesh*> QFAModelLoader::LoadModelSeparate(const std::string& pFile)
{
    std::vector<QStaticMesh*> meshes;
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(pFile,
        aiProcess_CalcTangentSpace |
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType |
        aiProcess_FlipWindingOrder |
        aiProcess_GenSmoothNormals
    );

    if (nullptr == scene)
    {
        std::cout << "fail\n";
        ASSERT(false);
        return meshes;
    }
    EFileFormat fileFormat = GetFileFormat(scene);
    
    GetMesh(scene, scene->mRootNode, fileFormat, meshes);
    importer.FreeScene();
    return meshes;
}

void QFAModelLoader::GetMesh(const aiScene* scene, const aiNode* node, EFileFormat fileFormat, std::vector<QStaticMesh*>& vector)
{
    for (size_t i = 0; i < node->mNumChildren; i++)
    {   
        unsigned int allIndexCound = 0;
        unsigned int verticeCound = 0;
        unsigned int unicueIndex = 0;

        CalculateIndex(scene, node->mChildren[i], allIndexCound, verticeCound, false);

        MeshData* mesDataReal = new MeshData(verticeCound, allIndexCound, scene->mNumMaterials, 0);
        unsigned int* meshIndexes = mesDataReal->GetIndexData();

        allIndexCound = 0;
        WriteIndex(scene, node->mChildren[i], meshIndexes, unicueIndex, allIndexCound, false);

        SSVertexMaterial* meshVertex = mesDataReal->GetFrameData(0);
        verticeCound = 0;

        WriteVertex(scene, node->mChildren[i], verticeCound, meshVertex, fileFormat, false);
        WriteMaterial(scene, node->mChildren[i], mesDataReal, false);
        vector.push_back(new QStaticMesh(mesDataReal));

        for (size_t j = 0; j < node->mChildren[i]->mNumChildren; j++)
            GetMesh(scene, node->mChildren[i]->mChildren[j], fileFormat, vector);
    }    
}
