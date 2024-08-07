#include "pch.h"
#include "StaticMeshActor.h"
QFAEngineClassOut(AStaticMeshActor);
AStaticMeshActor::AStaticMeshActor()
{
	SetRootComponent(&Mesh, true);
}

AStaticMeshActor::~AStaticMeshActor()
{
	SeparateRootComponent();
}

void AStaticMeshActor::SetMesh(QFAMeshData* md)
{
	Mesh.SetMesh(md);
	Mesh.SetWorldPosition(Position);
}
