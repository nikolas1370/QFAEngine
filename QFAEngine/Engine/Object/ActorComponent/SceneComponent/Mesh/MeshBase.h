#pragma once
#include <Math/Vector.h>
#include <Object/ActorComponent/SceneComponent/SceneComponent.h>

#include <vulkan/vulkan_core.h>
#include <array>
#include <Render/Window/Window.h>
#include <Render/vk/TextureSampler.h>
#include <Render/vk/ImageView.h>
#include <Render/Pipline/Pipline.h>

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

class QStaticMesh;
class QMeshBaseComponent;
class QFAVKVertexBuffer;
class QFAVKIndexBuffer;
class QFAEXPORT MeshData
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
	friend QStaticMesh;
	friend QMeshBaseComponent;
public:
	struct SMeshInfo
	{
		size_t FrameSize;
		size_t VertexCount;
		size_t VerticesSize;
		size_t UniqueIndexCount;
		size_t IndexCount;
		size_t MaterialCount;
		size_t AmountData;
	};
private:
	SMeshInfo Mi;
	QFAVKVertexBuffer* VertexBufer = nullptr;
	QFAVKIndexBuffer* IndexBuffer = nullptr;
	// call when set MeshData in staticMesh
	void CreateVertextIndexBuffer();
public:


	/*
		uniqueIndexCount = count SSVertex in memory
		indexCount = count of indices in list who represent all of mesh
	*/	
	MeshData( int uniqueIndexCount, int indexCount, int materialCount);// false to true
	MeshData(int VertexCount, int indexCount, int materialCount, int notNed);	
	MeshData(SMeshInfo* mi, void* framesData);



	~MeshData()
	{
		delete VertexBufer;
		delete IndexBuffer;
		free((void*)FramesData);		
	}

	SSVertexMaterial* GetFrameData() const;
	SMeshInfo GetMeshInfo() const;
	inline unsigned int* GetIndexData() const
	{
		return (unsigned int*)&FramesData[Mi.FrameSize ];
	}

	inline Material* GetMaterialData() const
	{
		return (Material*)&FramesData[Mi.FrameSize + sizeof(unsigned int) * Mi.IndexCount];
	}

	inline void SetMaterial(Material material, int index)
	{
		((Material*)&FramesData[Mi.FrameSize  + sizeof(unsigned int) * Mi.IndexCount])[index] = material;
	}


	inline unsigned char GetMaterialCount() const
	{
		return Mi.MaterialCount;
	}
	

	inline int GetIndexCount() const
	{
		return Mi.IndexCount;
	}

	inline size_t GetVertexCount() const
	{
		return Mi.VertexCount;
	}

	// size of entire vertex in bytes 
	inline size_t GetVerticesSize() const
	{
		return Mi.VerticesSize;
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
class QFAOverlord;
#include <Render/Buffer/IndexBuffer.h>
#include <Render/Buffer/VertexBuffer.h>


class QFAEXPORT QMeshBaseComponent : public QSceneComponent
{
	friend QStaticMesh;
	friend QFAWindow;
	friend QFAViewport;
	friend QFAOverlord;

protected:
	

	bool CastShadow = true;

	glm::mat4 ModelMatrix = Math::DefauldMatrix4;
	void UpdateModelMatrix() override;

	struct SPushConstantPickId
	{
		unsigned int meshId;
	};

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


	virtual void UpdateBuffers(VkCommandBuffer commandBuffer, uint64_t startFrameTime, bool isShadow, const FVector& cameraPos) = 0;
	

	void* GetModelBuffer();
	
	void* GetFragmentBuffer();
	
	void* GetShadowBuffer();
	


	std::array<VkDescriptorSet, 2> GetNextSets();

	VkDescriptorSet GetShadowNextSet();
	


	struct SShaderDirLight
	{
		alignas(16) FVector direction = FVector(0.0f, 0.0f, 1.0f);
		alignas(16) FVector ambient = FVector(0.1f);
		alignas(16) FVector diffuse = FVector(1.0f);
		alignas(16) FVector specular = FVector(1.0f);
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

	
	static void createDescriptorSet0(VkBuffer buffer, VkBuffer shadeowBuffer);
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

	static  VkDescriptorSet descriptorSetShadow; 


	struct SSet1Buffers
	{
		QFAVKBuffer* vertexBuffer;
		QFAVKBuffer* fragmentBuffer;
	};

	static unsigned int SetsInUse ; // in one frame
	
	static const unsigned int DescriptorSets1Amount = 100;
	static const unsigned int DescriptorSets0Amount = 10; // for viewport set

	static std::vector<SSet1Buffers> Set1Buffers;

	static std::vector<VkDescriptorSet> ShadowDescriptorSets;


	static unsigned int MaxMeshId;
	static std::vector<QMeshBaseComponent*> MeshIdList;
	SPushConstantPickId PickId;
	// call only if Mf != nullptr
	void Render(VkCommandBuffer commandBuffer, bool shadow, FVector cameraPosition);
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



	static void StartFrameViewpoet(glm::mat4& viewPortProjection, glm::mat3& cameraRotationMatrix,  glm::mat4& directionLightMatrix, int viewportIndex);

};