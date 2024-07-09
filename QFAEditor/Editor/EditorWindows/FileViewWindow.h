#pragma once
#include <Tools/File/FileSystem.h>
#include <Object/World/World.h>
#include <Object/ActorComponent/SceneComponent/Mesh/MeshBase.h>
#include <Object/ActorComponent/SceneComponent/Mesh/StaticMesh.h>
#include <Object/Actor/Actor.h>
#include <Camera/CameraEditor.h>
#include <EditorFileStorage.h>
#include <Overlord/ContentManager.h>

class QFAWindow;
class QWorld;
class QFAImage;
class QWorld;
class QFAImage;
class QFAUICanvas;
class QFAUIList;
class QFAUIImage;
class QFAUIScroll;
class QFAEditorUIFileViewUnit;
class QFAEditorFileViewWindow // this window display QFAfile
{
	struct SFile
	{
		size_t FileId; // before QFAContentManager::QFAContentFile ef;
		QFAEditorUIFileViewUnit* viewUnit;
	};

	enum ECurentType
	{
		CTNone,
		CTMesh,
		CTImage
	};

	bool Closed = false;
	QFAWindow* Window;
	ACameraEditor Camera;
	QWorld World;
	QActor Actor;
	QStaticMesh Mesh;

	QFAUIImage* DisplayImage;

	QFAUICanvas* Canvas;
	QFAUICanvas* TopListCanvas;

	QFAUIList* TopList;
	QFAUIScroll* TopListScroll;
	int CurentEnableFolderCount = 0;
	std::vector<SFile> Files;
	QFAEditorUIFileViewUnit* CurentViewUnit = nullptr;

	ECurentType CurentActiveType = ECurentType::CTNone;

	static void LeftMouse(QFAUIUnit* unit, void* _this);
	static void RightMouse(QFAUIUnit* unit, void* _this);

	void InFocus(SFile* file);
	void UpdateList();
public:
	QFAEditorFileViewWindow();
	~QFAEditorFileViewWindow();
	
	bool IsClosed();
	void AddFile(size_t fileId);
};