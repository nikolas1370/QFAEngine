#include "StaticMesh.h"
#include <Tools/VulkanSuff.h>

QStaticMesh::QStaticMesh(MeshData* meshData)
{
	SetMesh(meshData);
}

void QStaticMesh::SetMesh(MeshData* meshData)
{
	if (!meshData)
	{
		Mf = nullptr;
		return;
	}

	Mf = meshData;
	Mf->CreateVertextIndexBuffer();
}

void QStaticMesh::UpdateBuffers(VkCommandBuffer commandBuffer, uint64_t startFrameTime, bool isShadow, const FVector &cameraPos)
{	
	// convertation from 1 unit 1 centimeter to 1 unit 1 meter.
	ModelMatrix[3][0] = (WorldPosition.X - cameraPos.X) * 0.01f;
	ModelMatrix[3][1] = (WorldPosition.Y - cameraPos.Y) * 0.01f;
	ModelMatrix[3][2] = (WorldPosition.Z - cameraPos.Z) * 0.01f;
	if (isShadow)
	{
		QMeshBaseComponent::UBOShadowVertex ubo{};
		
		ubo.depthMVP = LightMatrix * ModelMatrix;
		memcpy(GetShadowBuffer(), &ubo.depthMVP, sizeof(ubo));
	}
	else
	{
		memcpy(GetModelBuffer(), &ModelMatrix, sizeof(ModelMatrix));

		void* faragbuf = GetFragmentBuffer();

		char* tem = (char*)faragbuf + sizeof(SShaderDirLight);

		memcpy(tem, Mf->GetMaterialData(), sizeof(Material) * Mf->GetMaterialCount()); // for material create new buffer

		memcpy(faragbuf, &ShaderDL, sizeof(SShaderDirLight));
	}

}

int QStaticMesh::GetIndexCount()
{
	return Mf->GetIndexCount();
}
