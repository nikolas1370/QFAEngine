#pragma once
#include <EngineStuff/Image.h>
#include <Object/ActorComponent/SceneComponent/Mesh/MeshBase.h>
#include <Window/Window.h>
#include <EngineStuff/Window/ViewportHolder.h>

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