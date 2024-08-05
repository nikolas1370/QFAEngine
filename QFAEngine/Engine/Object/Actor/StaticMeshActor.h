#pragma once
#include <Object/Actor/Actor.h>
#include <Object/ActorComponent/SceneComponent/Mesh/StaticMesh.h>

class QFALevel;
class QFAEXPORT AStaticMeshActor : public QActor
{
	QFAEngineClassIn();
	friend QFALevel;
	friend QFAClass;
protected:
	QStaticMesh Mesh;

public:
	AStaticMeshActor();
	~AStaticMeshActor();

	void SetMesh(QFAMeshData* md);
};