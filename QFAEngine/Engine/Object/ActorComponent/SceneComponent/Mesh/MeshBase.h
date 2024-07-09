#pragma once
#include <Math/Vector.h>
#include <Object/ActorComponent/SceneComponent/SceneComponent.h>

#include <vulkan/vulkan_core.h>
#include <array>
#include <Render/Window/Window.h>
#include <Render/vk/TextureSampler.h>
#include <Render/vk/ImageView.h>
#include <Render/Pipline/Pipline.h>
#include <Render/Buffer/IndexBuffer.h>
#include <Render/Buffer/VertexBuffer.h>
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
	int MaterialIndex = 0;
};

struct Material // struct in fragment shader
{
	FVector Color = FVector(1, 1, 1);
	float Specular = 0;
};

class QStaticMesh;
class QMeshBaseComponent;
class QFAContentManager;
class QFAFile;
class QFAEXPORT QFAMeshData
{	
	friend QStaticMesh;
	friend QMeshBaseComponent;
	friend QFAContentManager;
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

	/*
	in memory
	{ without Interpolation
		[SSVertexFrame1,.., SSVertexFrameLast] // successively frame after frame
		Indexes,
		Materials
	}
	*/
	char* FramesData;
	QFAFile* QFile = nullptr; // if QFile == null free(FramesData) else delete QFile

	SMeshInfo Mi;
	QFAVKVertexBuffer* VertexBufer = nullptr;
	QFAVKIndexBuffer* IndexBuffer = nullptr;
	
#if QFA_EDITOR_ONLY
	// all QMeshBaseComponent why use this QFAMeshData need for update mesh
	std::vector<QMeshBaseComponent*> Meshs; 
	void DeleteMeFromList(QMeshBaseComponent* mesh)
	{
		for (size_t i = 0; i < Meshs.size(); i++)
		{
			if (Meshs[i] == mesh)
			{
				Meshs.erase(Meshs.begin() + i);
				return;
			}
		}
	}
#endif
	// not forget call it's if create QFAMeshData and set all data
	void CreateVertextIndexBuffer();
	QFAMeshData(int VertexCount, int indexCount, int materialCount, int notNed);
	QFAMeshData(SMeshInfo* mi, void* framesData, QFAFile* file);
	~QFAMeshData()
	{
		DeleteData();
	}

	void DeleteData()
	{
		if (VertexBufer)
			delete VertexBufer;

		if (IndexBuffer)
			delete IndexBuffer;
				
		if (FramesData)
		{
			if (QFile)
				delete QFile;
			else
				free(FramesData);
		}
	}

protected:
	// child class can use this
	static QFAMeshData* CreateMeshData(int VertexCount, int indexCount, int materialCount, int notNed)
	{
		return new QFAMeshData(VertexCount, indexCount, materialCount, notNed);
	}
	// child class can use this
	static QFAMeshData* CreateMeshData(SMeshInfo* mi, void* framesData, QFAFile* file)
	{
		return new QFAMeshData(mi, framesData, file);
	}
	
	static void operator delete (void* p)
	{
		delete p;
	}

	inline void SetMaterial(Material material, int index)
	{
		((Material*)&FramesData[Mi.FrameSize + sizeof(unsigned int) * Mi.IndexCount])[index] = material;
	}

	inline Material* GetDefaultMaterials() const
	{
		return (Material*)&FramesData[Mi.FrameSize + sizeof(unsigned int) * Mi.IndexCount];
	}

#if QFA_EDITOR_ONLY
	/*
		in end delete this

		need in QFAContentManager::AddFile
	*/
	void UpdateMeshData(QFAMeshData* meshData);
#endif

public:	

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

class QFAEXPORT QMeshBaseComponent : public QSceneComponent
{
	friend QStaticMesh;
	friend QFAWindow;
	friend QFAViewport;
	friend QFAOverlord;
	friend QFAMeshData;
private:
	struct SBufferVertex
	{
		VkBuffer BufferVertex;
		VkDeviceMemory BufferVertexMemory;
		void* BufferVertexMapped;
	};

	struct SSet1Buffers
	{
		QFAVKBuffer* VertexBuffer;
		QFAVKBuffer* FragmentBuffer;
	};

protected:	
	struct SShaderDirLight
	{
		alignas(16) FVector Direction = FVector(0.0f, 0.0f, 1.0f);
		alignas(16) FVector Ambient = FVector(0.1f);
		alignas(16) FVector Diffuse = FVector(1.0f);
		alignas(16) FVector Specular = FVector(1.0f);
	};

	struct UBOFragment
	{// DirLight
		SShaderDirLight DL;
		alignas(16) Material Material[101];
	};

	struct SPushConstantPickId
	{
		unsigned int MeshId;
	};

	struct UBOVertex
	{
		alignas(64) glm::mat4 Projection;// put in start mesh render		
		alignas(64) glm::mat4 CameraR;
		alignas(64) glm::mat4 DirectionLightMatrix;
	};

	struct UBOShadowVertex
	{
		alignas(64) glm::mat4 DepthMVP;
	};

private:
	static QFAVKPipeline* Pipeline;
	static QFAVKPipeline* ShadowPipline;
	static VkCommandPool CommandPool;
	static VkRenderPass RenderPass;
	static  VkRenderPass ShadowRenderPass;
	static  VkDescriptorPool DescriptorPool;
	static  std::vector<VkDescriptorPool> DescriptorPoolsTwo;
	static  std::vector<VkDescriptorPool> DescriptorPoolsShadow;
	static  VkDescriptorSet DescriptorSetShadow;
	static unsigned int SetsInUse; // in one frame
	static const unsigned int DescriptorSets1Amount = 100;
	static const unsigned int DescriptorSets0Amount = 10; // for viewport set
	static std::vector<SSet1Buffers> Set1Buffers;
	static std::vector<VkDescriptorSet> ShadowDescriptorSets;
	static unsigned int MaxMeshId;
	static std::vector<QMeshBaseComponent*> MeshIdList;
	static SShaderDirLight ShaderDL;

protected:
	static glm::mat4 LightMatrix;

private:
	SPushConstantPickId PickId;
	bool CastShadow = true;
	// if Materials == nulptr use Default Material
	Material* Materials = nullptr;
	glm::mat4 ModelMatrix = Math::DefauldMatrix4;
	QFAMeshData* Mf;

public:
	virtual int GetIndexCount() = 0;
	virtual void UpdateBuffers(VkCommandBuffer commandBuffer, uint64_t startFrameTime, bool isShadow, const FVector& cameraPos) = 0;

private:
	static void StartFrame();
	static void StartShadowFrameViewport(glm::mat4& lmat);
	static void EndLife();
	static void createDescriptorSet0(VkBuffer buffer, VkBuffer shadeowBuffer);
	static void createDescriptorSet1();
	static void Init(VkRenderPass renderPass, VkRenderPass ShadowRenderPass, VkCommandPool commandPool_);
	static void CreatePipeline();
	static void CreateShadowPipline();

protected:
	static void StartFrameViewpoet(glm::mat4& viewPortProjection, glm::mat3& cameraRotationMatrix, glm::mat4& directionLightMatrix, int viewportIndex);

private:
	void UpdateModelMatrix() override;
	inline Material* GetMaterials() const
	{
		return (Materials ? Materials : Mf->GetDefaultMaterials());
	}

	// call only if Mf != nullptr
	void Render(VkCommandBuffer commandBuffer, bool shadow, FVector cameraPosition);

protected:
	void ResetMaterials()
	{
		if (Materials)
		{
			delete Materials;
			Materials = nullptr;
		}
	}

public:	

	QMeshBaseComponent();
	~QMeshBaseComponent();
	virtual void SetMesh(QFAMeshData* meshData) = 0;
	inline void SetCastShadow(bool castShadow)
	{
		CastShadow = castShadow;
	}

	inline bool GetCastShadow()
	{
		return CastShadow;
	}

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
		{
			Materials = new Material[Mf->GetMaterialCount()];
#if QFA_EDITOR_ONLY
			Mf->Meshs.push_back(this);
#endif
		}

		Materials[index] = mat;
		return true;
	}

	inline size_t GetMaterialCount() const
	{
		return Mf->GetMaterialCount();
	}

	std::array<VkDescriptorSet, 2> GetNextSets();

	VkDescriptorSet GetShadowNextSet();
};