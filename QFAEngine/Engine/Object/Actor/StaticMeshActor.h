#pragma once
#include <Object/Actor/Actor.h>
#include <Object/ActorComponent/SceneComponent/Mesh/StaticMesh.h>
class AStaticMeshActor : public QActor
{
	QStaticMesh Mesh;
public:
	AStaticMeshActor();
	~AStaticMeshActor();

	void SetMesh(MeshData* md);
private:

};