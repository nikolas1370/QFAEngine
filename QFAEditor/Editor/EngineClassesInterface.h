#pragma once
#include <EngineStuff/Image.h>
#include <Object/ActorComponent/SceneComponent/Mesh/MeshBase.h>
#include <Window/Window.h>
#include <EngineStuff/Window/ViewportHolder.h>
#include <Object/World/Level.h>
#include <Object/Actor/Actor.h>
#include <UI/Text.h>
#include <UI/Canvas.h>
#include <UI/UIList.h>
#include <UI/UIImage.h>
#include <Object/Actor/StaticMeshActor.h>

class QFAEditorFileStorage;
class QFAEditorExplorerFolderUnit;

class QFAEditorImage : public QFAImage
{
	friend QFAEditorExplorerFolderUnit;
	friend QFAEditorFileStorage;
};

class QFAEditorMainWindow;
class QFAModelLoader;
class QFAEditorMeshData : public QFAMeshData
{
	friend QFAEditorMainWindow;
	friend QFAModelLoader;
};

class QFAEditorFileViewWindow;
class QFAEditorWindow : public QFAWindow
{
	friend QFAEditorMainWindow;
	friend QFAEditorFileViewWindow;
};

class QFAEditorViewportHolder : public QFAViewportHolder
{
	friend QFAEditorMainWindow;
};

class QFAEditorMainWindow;
class QFAEditorLevel : public QFALevel
{
	friend QFAEditorMainWindow;
	QFAEditorLevel(std::u32string path)
	{
		LevelPath = path;
	}
};

class QFAEditorOverlord;

class QFAEgitorClass : public QFAClass
{
	friend QFAEditorOverlord;
};

class QFAEditorActor : public QActor
{
	friend QFAEditorMainWindow;
};
class QFAEditorUIFileViewUnit;

class QFAEditorText : public QFAText
{
	friend QFAEditorUIFileViewUnit;
};

class QFAUIActorTransform;
class QFAUIEditorFileExplorer;
class QFAEditorGameViewportInfo;
class QFAEditorCanvas : public QFAUICanvas
{
	friend QFAUIActorTransform;
	friend QFAUIEditorFileExplorer;
	friend QFAEditorExplorerFolderUnit;
	friend QFAEditorGameViewportInfo;
};

class QFAEditorList : public QFAUIList
{
	friend QFAUIEditorFileExplorer;
};

class QFAEditorUIImage : public QFAUIImage
{
	friend QFAUIEditorFileExplorer;
};

class AEditorStaticMeshActor : public AStaticMeshActor
{
	friend QFAEditorMainWindow;
};

class QEditorStaticMesh : public QStaticMesh
{
	friend QFAEditorMainWindow;
};