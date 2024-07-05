#pragma once
#include <Tools/File/FileSystem.h>
#include <Object/World/World.h>
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


#include <Object/ActorComponent/SceneComponent/Mesh/MeshBase.h>
#include <Object/ActorComponent/SceneComponent/Mesh/StaticMesh.h>
#include <Object/Actor/Actor.h>
#include <Camera/CameraEditor.h>
#include <EditorFileStorage.h>
#include <Overlord/ContentManager.h>
class QFAEditorFileViewWindow
{
	bool Closed = false;
	QFAWindow* Window;
	ACameraEditor Camera;
	QWorld World;
	QActor Actor;
	QStaticMesh Mesh;
public:
	QFAEditorFileViewWindow();
	~QFAEditorFileViewWindow();
	
	bool IsClosed();

	void AddFile(size_t fileId);
private:
	

	QFAUIImage* DisplayImage;
	
	QFAUICanvas* Canvas;
	QFAUICanvas* TopListCanvas;

	QFAUIList* TopList;
	QFAUIScroll* TopListScroll;

	
	struct SFile
	{
		QFAContentManager::QFAContentFile ef;
		/*
		std::u32string path;
		QStaticMesh* mesh = nullptr;
		QFAImage* image = nullptr;
		*/
		/* все зверху удалить знизу розкоментірувать  */
		//SEditorFile ef;
		QFAEditorUIFileViewUnit* viewUnit;	
	};
	

	enum ECurentType
	{
		CTNone,
		CTMesh,
		CTImage
	};

	int CurentEnableFolderCount = 0;
	std::vector<SFile> Files;
	QFAEditorUIFileViewUnit* CurentViewUnit = nullptr;

	ECurentType CurentActiveType = ECurentType::CTNone;

	void InFocus(SFile* file);
	void UpdateList();

	static void LeftMouse(QFAUIUnit* unit, void* _this);
	static void RightMouse(QFAUIUnit* unit, void* _this);
};