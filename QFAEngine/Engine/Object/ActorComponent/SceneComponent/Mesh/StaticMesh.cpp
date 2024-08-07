#include "pch.h"
#include "StaticMesh.h"
#include <EngineStuff/VulkanSuff.h>
#include <Window/Viewport.h>
QFAEngineClassOut(QStaticMesh);
QStaticMesh::QStaticMesh(QFAMeshData* meshData)
{
	SetMesh(meshData);
}

void QStaticMesh::SetMesh(QFAMeshData* meshData)
{
	if (!meshData)
	{
		Mf = nullptr;
		return;
	}
#if QFA_EDITOR_ONLY
	if(!QFAEngineViewport::GetInGame())
	{
		meshData->Meshs.push_back(this);
		if (Mf)
			Mf->DeleteMeFromList(this);
	}
#endif
	Mf = meshData;
	ResetMaterials();
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
		
		ubo.DepthMVP = LightMatrix * ModelMatrix;
		memcpy(GetShadowBuffer(), &ubo.DepthMVP, sizeof(ubo));
	}
	else
	{
		memcpy(GetModelBuffer(), &ModelMatrix, sizeof(ModelMatrix));

		void* faragbuf = GetFragmentBuffer();

		char* tem = (char*)faragbuf + sizeof(SShaderDirLight);

		memcpy(tem, GetMaterials(), sizeof(Material) * Mf->GetMaterialCount()); // for material create new buffer

		memcpy(faragbuf, &ShaderDL, sizeof(SShaderDirLight));
	}

}

int QStaticMesh::GetIndexCount()
{
	return Mf->GetIndexCount();
}
