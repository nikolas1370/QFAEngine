#pragma once
#include <string>
#include <vector>
#include <Tools/File/FileSystem.h>
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
#include <Object/Actor/Camera/CameraEditor.h>
class QFAEditorFileViewWindow
{
	bool Closed = false;
	QFAWindow* Window;
	ACameraEditor Camera;
	QWorld World;
	QActor Actor;
public:
	QFAEditorFileViewWindow();
	~QFAEditorFileViewWindow();
	
	bool IsClosed();

	void AddFile(std::u32string qfaFilePAth);
private:
	

	QFAUIImage* DisplayImage;
	
	QFAUICanvas* Canvas;
	QFAUICanvas* TopListCanvas;

	QFAUIList* TopList;
	QFAUIScroll* TopListScroll;

	struct SFile
	{
		std::u32string path;
		QStaticMesh* mesh = nullptr;
		QFAImage* image = nullptr;
		QFAEditorUIFileViewUnit* viewUnit;
		QFAFile* file;
	};

	enum ECurentType
	{
		CTNone,
		CTMesh,
		CTImage
	};

	int CurentEnableFolderCount = 0;
	std::vector<SFile> Folders;
	QFAEditorUIFileViewUnit* CurentViewUnit = nullptr;

	ECurentType CurentActiveType = ECurentType::CTNone;

	void InFocus(SFile* file);
	bool LoadFile(std::u32string qfaFilePAth, SFile & sfile);
	void UpdateList();

	static void LeftMouse(QFAUIUnit* unit, void* _this);
	static void RightMouse(QFAUIUnit* unit, void* _this);
};