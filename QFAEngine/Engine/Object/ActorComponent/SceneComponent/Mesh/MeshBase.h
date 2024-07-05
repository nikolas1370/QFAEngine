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
class QFAEXPORT QFAMeshData
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
	// call when set QFAMeshData in staticMesh
	void CreateVertextIndexBuffer();
	/*
	static void operator delete (void* p) 
	{
		delete ((QFAMeshData*)p)->VertexBufer;
		delete ((QFAMeshData*)p)->IndexBuffer;
		free(((QFAMeshData*)p)->FramesData);
		delete p;
	}
	*/


	inline Material* GetDefaultMaterials() const
	{
		return (Material*)&FramesData[Mi.FrameSize + sizeof(unsigned int) * Mi.IndexCount];
	}


public:
	QFAMeshData(int VertexCount, int indexCount, int materialCount, int notNed);
	QFAMeshData(SMeshInfo* mi, void* framesData);

	
	// move in private
	inline void SetMaterial(Material material, int index)
	{
		((Material*)&FramesData[Mi.FrameSize + sizeof(unsigned int) * Mi.IndexCount])[index] = material;
	}
	
	~QFAMeshData()
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

	inline Material GetDefaultMaterial(size_t index) const
	{
		if (Mi.MaterialCount >= index)
			return Material(0);

		return GetDefaultMaterials()[index];
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

	// if Materials == nulptr use Default Material
	Material* Materials = nullptr;

	inline Material* GetMaterials() const
	{
		return (Materials ? Materials : Mf->GetDefaultMaterials());
	}
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
	
	
	inline Material GetMaterial(size_t index) const
	{
		if (index >= Mf->GetMaterialCount())
			return Material(0);

		return (Materials ? Materials[index] : Mf->GetDefaultMaterials()[index]);
	}

	inline bool SetMaterial(Material mat, size_t index)
	{
		if (index >= Mf->GetMaterialCount())
			return false;

		if (!Materials)
			Materials = new Material[Mf->GetMaterialCount()];

		Materials[index] = mat;
		return true;
	}

	inline size_t GetMaterialCount() const
	{
		return Mf->GetMaterialCount();
	}

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

	QFAMeshData* Mf;

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