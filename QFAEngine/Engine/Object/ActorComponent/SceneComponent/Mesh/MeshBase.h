#pragma once
#include <Math/Vector.h>
#include <Object/ActorComponent/SceneComponent/SceneComponent.h>

#include <vulkan/vulkan_core.h>
#include <array>
#include <Render/Window/Window.h>
#include <Render/vk/TextureSampler.h>
#include <Render/vk/ImageView.h>
#include <Render/Pipline/Pipline.h>

/*


remove 
ShadowDescriptorSets


*/




struct VertexMaterial// 
{
	FVector Position;
	FVector Normal;
	FVector Color = FVector(1, 1, 1);
	float Specular = 0;
};

struct SSVertexMaterial // structure input vertex Shader 
{
	FVector Position;
	FVector Normal;
	int materialIndex = 0;
};

struct Material // struct in fragment shader
{
	FVector Color = FVector(1, 1, 1);
	float Specular = 0;
};

class MeshData
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

	size_t FrameSize; 
	size_t VertexCount;
	size_t VerticesSize;
	int UniqueIndexCount;
	int IndexCount;
	unsigned char MaterialCount;

public:


	/*
		uniqueIndexCount = count SSVertex in memory
		indexCount = count of indices in list who represent all of mesh
	*/
	MeshData( int uniqueIndexCount, int indexCount, int materialCount);// false to true
	MeshData(int VertexCount, int indexCount, int materialCount, int notNed);

	~MeshData()
	{
	
		free((void*)FramesData);
		
	}

	SSVertexMaterial* GetFrameData(int frame) const;
	inline unsigned int* GetIndexData() const
	{
		return (unsigned int*)&FramesData[FrameSize ];
	}

	inline Material* GetMaterialData() const
	{
		return (Material*)&FramesData[FrameSize + sizeof(unsigned int) * IndexCount];
	}

	inline void SetMaterial(Material material, int index)
	{
		((Material*)&FramesData[FrameSize  + sizeof(unsigned int) * IndexCount])[index] = material;
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
class QFAWindow;
class QFAVKPipeline;
class QStaticMesh;

#include <Render/Buffer/IndexBuffer.h>
#include <Render/Buffer/VertexBuffer.h>


class QMeshBaseComponent : public QSceneComponent 
{
	friend QStaticMesh;
	friend QFAWindow;
	friend QFAViewport;
	
protected:
	

	bool CastShadow = true;

	glm::mat4 ModelMatrix = Math::DefauldMatrix4;
	void UpdateModelMatrix() override;



public:	

	QMeshBaseComponent();
	~QMeshBaseComponent();

	inline void SetCastShadow(bool castShadow)
	{
		CastShadow = castShadow;
	}

	inline bool GetCastShadow()
	{
		return CastShadow;
	}
	
	
	virtual int GetIndexCount() = 0;


	virtual void UpdateBuffers( uint64_t startFrameTime, bool isShadow, const FVector& cameraPos) = 0;
	

	inline void* GetModelBuffer()
	{
		if (SetsInUse >= Set1Buffers.size())
			createDescriptorSet1();

		return Set1Buffers[SetsInUse].vertexBuffer->MapData;
	}

	inline void* GetFragmentBuffer()
	{
		if (SetsInUse >= Set1Buffers.size())
			createDescriptorSet1();
		
		return Set1Buffers[SetsInUse].fragmentBuffer->MapData;
	}

	inline void* GetShadowBuffer()
	{	
		return ShadowSetBuffers[ShadowSetsInUse]->MapData;		
	}

 	inline std::array<VkDescriptorSet, 2> GetNextSets()
	{
		return std::array<VkDescriptorSet, 2>
		{
			Pipeline->GetSet(0, QFAWindow::GetMainWindow()->ViewportProcess),
			Pipeline->GetSet(1, SetsInUse++)
		};
	}

	inline VkDescriptorSet GetShadowNextSet()
	{
		return ShadowPipline->GetSet(0, ShadowSetsInUse++);		
	}


	struct SShaderDirLight
	{
		alignas(16) FVector direction = FVector(0, 0, 1);
		alignas(16) FVector ambient = FVector(0.1);
		alignas(16) FVector diffuse = FVector(1);
		alignas(16) FVector specular = FVector(1);
	};

	struct UBOFragment
	{// DirLight
		SShaderDirLight DL;
		alignas(16) Material material[101];
	};

protected:
	static SShaderDirLight ShaderDL;

private:
	static void StartFrame();
	static void StartShadowFrameViewport(glm::mat4& lmat);

	
	static void EndLife();

	
	static void createDescriptorSets0();
	static void createDescriptorSet1();


	static void Init(VkRenderPass renderPass, VkRenderPass ShadowRenderPass, VkCommandPool commandPool_);
	

	static void CreatePipeline();
	static void CreateShadowPipline();

	static QFAVKPipeline* Pipeline;
	static QFAVKPipeline* ShadowPipline;



	static VkCommandPool commandPool;
	static VkRenderPass RenderPass;
	static  VkRenderPass ShadowRenderPass;


	static  VkDescriptorPool descriptorPool;
	static  std::vector<VkDescriptorPool> descriptorPoolsTwo;
	static  std::vector<VkDescriptorPool> descriptorPoolsShadow;

	struct SBufferVertex
	{
		VkBuffer BufferVertex;
		VkDeviceMemory BufferVertexMemory;
		void* BufferVertexMapped;
	};

	static std::array<QFAVKBuffer*, QFAWindow::MaxActiveViewPort> BuffersVertex;

	

	static  VkDescriptorSet descriptorSetShadow; 

	


	QFAVKVertexBuffer* VertexBufer;
	QFAVKIndexBuffer* IndexBuffer;

	struct SSet1Buffers
	{
		QFAVKBuffer* vertexBuffer;
		QFAVKBuffer* fragmentBuffer;
	};




	static unsigned int SetsInUse ; // in one frame
	
	static const unsigned int DescriptorSets1Amount = 100;

	static std::vector<SSet1Buffers> Set1Buffers;

	static unsigned int ShadowSetsInUse; 
	static std::vector<VkDescriptorSet> ShadowDescriptorSets;
	static std::vector<QFAVKBuffer*> ShadowSetBuffers;
	
	

	

protected:

	static glm::mat4 LightMatrix;

	MeshData* Mf;

	struct UBOVertex
	{
		alignas(64) glm::mat4 projection;// put in start mesh render		
		alignas(64) glm::mat4 cameraR;
		alignas(64) glm::mat4 directionLightMatrix;
	};



	struct UBOShadowVertex
	{
		alignas(64) glm::mat4 depthMVP;
	};


	void CreateVertexIndexBuffers();

	static void StartFrameViewpoet(glm::mat4& viewPortProjection, glm::mat3& cameraRotationMatrix,  glm::mat4& directionLightMatrix);

};