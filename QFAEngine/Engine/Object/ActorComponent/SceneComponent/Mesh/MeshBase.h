#pragma once
#include <Math/Vector.h>
#include <Object/ActorComponent/SceneComponent/SceneComponent.h>
#include <Render/Shader/ShaderProgram.h>

struct VertexMaterial// in future be replace to VertexMaterialNew 
{
	FVector Position;
	FVector Normal;
	FVector Color = FVector(1, 1, 1);
	float Specular = 0;
};

struct SSVertexMaterial // Shader structure vertex
{
	FVector Position;
	FVector Normal;
	int materialIndex;
};

struct Material
{
	FVector Color = FVector(1, 1, 1);
	float Specular = 0;
};

class MeshFrames
{
	/*
	in memory
	{ without Interpolation
		[SSVertexFrame1,.., SSVertexFrameLast] // successively frame after frame
		Indexes,
		Materials
	}
	*/
	char* FramesData;
	int FrameCount;
	size_t FrameSize;
	size_t VertexCount;
	size_t VerticesSize; // size all vertex in byte
	int UniqueIndexCount;
	int IndexCount;
	unsigned char MaterialCount;
	unsigned int FramePerSecond;
	double FrameTime;//  1.0 / FramePerSecond
	double AnimationTime;

	bool Interpolation; // delete

	

public:


	/*
		uniqueIndexCount = count SSVertex in memory
		indexCount = count of indices in list who represent all of mesh
	*/
	MeshFrames(int frameCount, int uniqueIndexCount, int indexCount, int materialCount, unsigned int framePerSecond, bool interpolation = true);// false to true


	~MeshFrames()
	{
		std::cout << "~MeshFrames\n";
		free((void*)FramesData);
		
	}

	SSVertexMaterial* GetFrameData(int frame) const;
	inline unsigned int* GetIndexData() const
	{
		return (unsigned int*)&FramesData[FrameSize * FrameCount];
	}

	inline Material* GetMaterialData() const
	{
		return (Material*)&FramesData[FrameSize * FrameCount + sizeof(unsigned int) * IndexCount];
	}

	inline void SetMaterial(Material material, int index)
	{
		((Material*)&FramesData[FrameSize * FrameCount + sizeof(unsigned int) * IndexCount])[index] = material;
	}

	inline double GetFrameTime() const
	{
		return FrameTime;
	}

	inline double GetAnimationTime() const
	{
		return AnimationTime;
	}
	inline int GetFrameCount() const
	{
		return FrameCount;
	}

	inline size_t GetFrameSize() const
	{
		return FrameSize;
	}
	
	inline unsigned char GetMaterialCount() const
	{
		return MaterialCount;
	}
	

	inline int GetIndexCount() const
	{
		return IndexCount;
	}

	inline size_t GetVertexCount() const
	{
		return VertexCount;
	}

	// size of entire vertex in bytes 
	inline size_t GetVerticesSize() const
	{
		return VerticesSize;
	}

	// може прийдец€ удалить const
	inline SSVertexMaterial* GetVerticesDate() const
	{
		return (SSVertexMaterial*)FramesData;
	}
	
	
};



class QFAShaderProgram;
class QFAViewport;

class QMeshBaseComponent : public QSceneComponent // abstract class
{
	
	friend QFAViewport;
protected:
	
	unsigned int VAO;
	unsigned int VBO;
	unsigned int IBO;
	bool Init = false;
	bool CastShadow = true;

	
	glm::mat4 ModelMatrix = Math::DefauldMatrix4;
	void UpdateModelMatrix(bool onlyPosition) override;


	inline virtual QFAShaderProgram* GetShaderProgram() = 0;
	inline virtual QFAShaderProgram* GetShadowShaderProgram() = 0;
	
public:
	QMeshBaseComponent();

	inline void SetCastShadow(bool castShadow)
	{
		CastShadow = castShadow;
	}

	inline bool GetCastShadow()
	{
		return CastShadow;
	}
	
	
	virtual int GetIndexCount() = 0;// pure virtual method

	// remove ShaderProgram* SP
	virtual void Bind(uint64_t startFrameTime, bool isShadow ) = 0;

};
