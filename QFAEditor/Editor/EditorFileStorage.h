#pragma once
#include <Tools/File/FileSystem.h>
#include <string>
#include <Object/ActorComponent/SceneComponent/Mesh/StaticMesh.h>
#include <Render/Image.h>
#include <Overlord/ContentManager.h>
#include <vector>

class QFAEditorMainWindow;
class QFAUIEditorFileExplorer;
class QFAEditorOverlord;
class QFAEditorFileViewWindow;
class QFAEditorFileStorage : public QFAContentManager
{
	friend QFAEditorMainWindow;
	friend QFAUIEditorFileExplorer;
    friend QFAEditorOverlord;
    friend QFAEditorFileViewWindow;
    // <icu::UChar32>
    static std::vector<int> DropPath;

    static void DropFiles(size_t folderId, int path_count, const char* paths[]);
};
