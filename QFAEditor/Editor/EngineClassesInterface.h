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
#include <Object/ActorComponent/SceneComponent/Camera/Camera.h>
#include <EngineStuff/EngineTextLocalization.h>

class QFAGameCode;
class QFAEditorFileStorage;
class QFAEditorExplorerFolderUnit;
class QFAEditorMainWindow;
class QFAModelLoader;
class QFAEditorFileViewWindow;
class QFAEditorOptionWindow;
class QFAEditorMainWindow;
class QFAEditorOverlord;
class QFAEditorUIFileViewUnit;
class QFAUIActorTransform;
class QFAUIEditorFileExplorer;
class QFAEditorGameViewportInfo;

class QFAEditorImage : public QFAImage
{
	friend QFAEditorExplorerFolderUnit;
	friend QFAEditorFileStorage;
};


class QFAEditorMeshData : public QFAMeshData
{
	friend QFAEditorMainWindow;
	friend QFAModelLoader;
};


class QFAEditorWindow : public QFAWindow
{
	friend QFAEditorMainWindow;
	friend QFAEditorFileViewWindow;
	friend QFAEditorOptionWindow;
};

class QFAEditorViewportHolder : public QFAViewportHolder
{
	friend QFAEditorMainWindow;
};


class QFAEditorLevel : public QFALevel
{
	friend QFAEditorMainWindow;
	QFAEditorLevel(std::u32string path)
	{
		LevelPath = path;
	}
};


class QFAEgitorClass : public QFAClass
{
	friend QFAEditorOverlord;
};

class QFAEditorActor : public QActor
{
	friend QFAEditorMainWindow;
};


class QFAEditorText : public QFAText
{
	friend QFAEditorUIFileViewUnit;
};


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

class QEditorWorld : public QWorld
{
	friend QFAEditorMainWindow;
};

class QEditorCameraComponent : public QCameraComponent
{
	friend QFAEditorMainWindow;
};

class QFAEditorLocalization : public QFAEngineLocalization
{
	friend QFAGameCode;
};