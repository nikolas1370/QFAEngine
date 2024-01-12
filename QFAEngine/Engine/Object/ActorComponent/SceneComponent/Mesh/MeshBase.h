#pragma once
#include <Math/Vector.h>
#include <Object/ActorComponent/SceneComponent/SceneComponent.h>

#include <vulkan/vulkan_core.h>
#include <array>
#include <Render/Window/Window.h>
#include <Render/vk/TextureSampler.h>
#include <Render/vk/ImageView.h>

class QFAVKMeshShadowPipeline;

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
class QFAVKMeshPipeline;
class QStaticMesh;
#include <Render/Pipline/MeshPipeline.h>
#include <Render/Buffer/IndexBuffer.h>
#include <Render/Buffer/VertexBuffer.h>


class QMeshBaseComponent : public QSceneComponent // abstract class
{
	friend QStaticMesh;
	friend QFAWindow;
	friend QFAViewport;
	friend QFAVKMeshPipeline;
protected:
	

	bool CastShadow = true;

	
	glm::mat4 ModelMatrix = Math::DefauldMatrix4;
	void UpdateModelMatrix(bool onlyPosition) override;



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



	virtual void UpdateBuffers( uint64_t startFrameTime, bool isShadow) = 0;
	




	inline void* GetModelBuffer()
	{
		if (SetsInUse >= Set1Buffers.size())
			createDescriptorPool1();

		return Set1Buffers[SetsInUse].BufferVertexModelMatrixMapped;
	}

	inline void* GetFragmentBuffer()
	{
		if (SetsInUse >= Set1Buffers.size())
			createDescriptorPool1();

		return Set1Buffers[SetsInUse].BufferFragmentMapped;
	}

	inline void* GetShadowBuffer()
	{
		if (ShadowSetsInUse >= ShadowSetBuffers.size())
			createDescriptorPoolShadow();

		return ShadowSetBuffers[ShadowSetsInUse].BufferShadowModelMatrixMapped;		
	}




 	inline std::array<VkDescriptorSet, 2> GetNextSets()
	{
		
		return std::array<VkDescriptorSet, 2>{QMeshBaseComponent::ViewportSets[QFAWindow::GetMainWindow()->ViewportProcess], DescriptorSets1[SetsInUse++]};
	}

	inline VkDescriptorSet GetShadowNextSet()
	{
		
		return ShadowDescriptorSets[ShadowSetsInUse++];
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

	static void StartShadowFrame();
	static void StartShadowFrameViewport(glm::mat4& lmat);

	
	static void EndLife();
	static void createDescriptorPool0();
	static void createDescriptorPool1();
	static void createDescriptorPoolShadow();
	static void createDescriptorSet0();
	static void createDescriptorSet1();
	static void createDescriptorSetShadow();
	

	static void Init(VkRenderPass renderPass, VkRenderPass ShadowRenderPass, VkCommandPool commandPool_);
	static VkVertexInputBindingDescription getBindingDescription();
	static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions();


	static QFAVKMeshPipeline* Pipeline;
	static QFAVKMeshShadowPipeline* ShadowPipline;



	static VkCommandPool commandPool;
	static VkRenderPass RenderPass;


	static  VkDescriptorPool descriptorPool;
	static  std::vector<VkDescriptorPool> descriptorPoolsTwo;
	static  std::vector<VkDescriptorPool> descriptorPoolsShadow;

	struct SBufferVertex
	{
		VkBuffer BufferVertex;
		VkDeviceMemory BufferVertexMemory;
		void* BufferVertexMapped;
	};

	
	static std::array<VkDescriptorSet, QFAWindow::MaxActiveViewPort> ViewportSets;
	static std::array<SBufferVertex, QFAWindow::MaxActiveViewPort> BuffersVertex;



	static  VkDescriptorSet descriptorSetShadow; 

	


	QFAVKVertexBuffer* VertexBufer;
	QFAVKIndexBuffer* IndexBuffer;

	struct SSet1Buffers
	{
		VkBuffer BufferVertexModelMatrix;
		VkDeviceMemory BufferVertexModelMatrixMemory;
		void* BufferVertexModelMatrixMapped;

		VkBuffer BufferFragment;
		VkDeviceMemory BufferFragmentMemory;
		void* BufferFragmentMapped;
	};


	struct SShadowSetBuffers
	{
		VkBuffer BufferShadowModelMatrix;
		VkDeviceMemory BufferShadowModelMatrixMemory;
		void* BufferShadowModelMatrixMapped;
	};

	static unsigned int SetsInUse ; // in one frame
	
	static const unsigned int DescriptorSets1Amount = 100;
	static std::vector<VkDescriptorSet> DescriptorSets1;
	static std::vector<SSet1Buffers> Set1Buffers;

	static unsigned int ShadowSetsInUse; 
	static std::vector<VkDescriptorSet> ShadowDescriptorSets;
	static std::vector<SShadowSetBuffers> ShadowSetBuffers;
	
	

	

protected:

	static glm::mat4 LightMatrix;

	MeshData* Mf;

	struct UBOVertex
	{
		alignas(64) glm::mat4 projection;// put in start mesh render		
		alignas(64) glm::mat4 cameraR;
		alignas(64) glm::mat4 directionLightMatrix;
		alignas(16) FVector cameraP;		
	};



	struct UBOShadowVertex
	{
		alignas(64) glm::mat4 depthMVP;
	};


	void CreateVertexIndexBuffers();

	static void StartFrameViewpoet(glm::mat4& viewPortProjection, glm::mat3& cameraRotationMatrix, FVector& cameraPosition, glm::mat4& directionLightMatrix);

};