#pragma once
#include <Object/Actor/Actor.h>
#include <Object/ActorComponent/SceneComponent/Mesh/StaticMesh.h>
class QFAEXPORT AStaticMeshActor : public QActor
{
	QStaticMesh Mesh;
public:
	AStaticMeshActor();
	~AStaticMeshActor();

	void SetMesh(QFAMeshData* md);
private:

};