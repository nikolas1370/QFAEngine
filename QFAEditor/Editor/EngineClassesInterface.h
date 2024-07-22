#pragma once
#include <EngineStuff/Image.h>
#include <Object/ActorComponent/SceneComponent/Mesh/MeshBase.h>
#include <Window/Window.h>
#include <EngineStuff/Window/ViewportHolder.h>
#include <Object/World/Level.h>
#include <Object/Actor/Actor.h>

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

class QFAEditorMainWindow;
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