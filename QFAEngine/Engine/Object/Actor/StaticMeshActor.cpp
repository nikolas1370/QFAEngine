#include "pch.h"
#include "StaticMeshActor.h"

AStaticMeshActor::AStaticMeshActor()
{
	SetRootComponent(&Mesh, true);
}

AStaticMeshActor::~AStaticMeshActor()
{
}

void AStaticMeshActor::SetMesh(MeshData* md)
{
	Mesh.SetMesh(md);
}
