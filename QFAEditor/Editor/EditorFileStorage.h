#pragma once
#include <Tools/File/FileSystem.h>
#include <EditorFileTypes.h>
#include <string>
#include <Object/ActorComponent/SceneComponent/Mesh/StaticMesh.h>
#include <Render/Image.h>
#include <stb_image.h>
#include <vector>
class QFAEditorMainWindow;
class QFAUIEditorFileExplorer;
class QFAEditorOverlord;


struct SEditorFile
{    
    std::u32string path;
    void* file = nullptr; // cast data to 
    size_t id;
    QFAEditorFileTypes fileType;
};

struct SFolder
{
    std::u32string path;
    std::vector<QFAFileSystem::FolderUnit> folderUnits;
    size_t id;
};


class QFAEditorFileStorage
{
	friend QFAEditorMainWindow;
	friend QFAUIEditorFileExplorer;
    friend QFAEditorOverlord;

    struct SEditorFileInside
    {
        std::u32string path;
        void* fileData = nullptr;
        QFAFile* file;
        size_t id;
        QFAEditorFileTypes type;
    };

    // store all folders
    static std::vector<SFolder> Folders;
	static std::vector<SEditorFileInside> EditorFile;//s

	static size_t Id;
    static size_t folderId;

    // LoadFilesInfolder
	static std::vector<QFAFileSystem::FolderUnit> FolderContents;
    // fpr LoadEditorFiles
    static std::vector<std::u32string> FoldersForWork;



public:
    static SEditorFile GetFile(size_t fileIndex);

private:	
    // textForDisplay display in load screen
    static void LoadEditorFiles(std::u32string& textForDisplay, bool& ifTextChange);
	
    
    static void GetFolderContents(size_t folderId, std::vector<QFAFileSystem::FolderUnit>& folderContents);
	


	static void EndLife()
	{

	}

    

/*--next function only for inside usage--*/

	static void LoadFilesInfolder(size_t folderIndex, std::u32string& textForDisplay, bool& ifTextChange);

	/*
		if in sfile.id == 0 editor file not support or invalid
	*/
	static void LoadFile(std::u32string qfaFilePAth, SEditorFileInside& sfile);
};
