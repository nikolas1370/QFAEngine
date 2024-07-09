#pragma once
#include <Render/Image.h>
#include <Object/ActorComponent/SceneComponent/Mesh/MeshBase.h>
class QFAEditorFileStorage;
class QFAEditorExplorerFolderUnit;

class QFAEditorImage : public QFAImage
{
	friend QFAEditorExplorerFolderUnit;
	friend QFAEditorFileStorage;
};

class QFAModelLoader;
class QFAEditorMeshData : public QFAMeshData
{
	friend QFAModelLoader;
};