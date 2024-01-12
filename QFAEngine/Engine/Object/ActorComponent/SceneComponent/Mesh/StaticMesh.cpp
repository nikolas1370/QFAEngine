#include "StaticMesh.h"
#include <Tools/Debug/OpenGlStuff.h>

QStaticMesh::QStaticMesh(MeshData* meshData)
{
	SetMesh(meshData);
}

void QStaticMesh::SetMesh(MeshData* meshData)
{
	Mf = meshData;
	CreateVertexIndexBuffers();
}

void QStaticMesh::UpdateBuffers( uint64_t startFrameTime, bool isShadow)
{		
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
