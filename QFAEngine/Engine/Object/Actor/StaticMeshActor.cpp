#include "pch.h"
#include "StaticMeshActor.h"
QFAEngineClassOut(AStaticMeshActor, QFAClass::ObjectClasses::StaticMeshActor);
AStaticMeshActor::AStaticMeshActor()
{
	SetRootComponent(&Mesh, true);
}

AStaticMeshActor::~AStaticMeshActor()
{
}

void AStaticMeshActor::SetMesh(QFAMeshData* md)
{
	Mesh.SetMesh(md);
	Mesh.SetWorldPosition(Position);
}
