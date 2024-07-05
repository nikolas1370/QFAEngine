#pragma once
#include <string>
#include <Tools/Stuff.h>
#include <Tools/File/FileSystem.h>
class QFAMeshData;
class QFAOverlord;

class QFAEXPORT QFAContentManager
{
	friend QFAOverlord;
protected:
	enum QFAFileTypes : short
	{
		EFTUndefined = 0,
		EFTMesh = 1,
		EFTImage = 2
	};

	struct QFAFileOnDisk // if create new version editor file create conventor from old version
	{
		static const unsigned short EditorFileVersion = 1;
		const char signature[10] = "QFAEditor";
		QFAFileTypes type;
		unsigned short version;
		QFAFileOnDisk() {}
		QFAFileOnDisk(QFAFileTypes fileType, unsigned int fileVersion)
		{
			type = fileType;
			version = fileVersion;
		}
	};

    struct QFAContentFolder // SFolder
    {
        std::u32string path;
        std::vector<QFAFileSystem::FolderUnit> folderUnits;
        size_t id;
    };
public:
	struct QFAContentFile // SEditorFile
	{
		std::u32string path;
		void* file = nullptr; // cast data to 
		size_t id;
		QFAFileTypes fileType;
	};
private:
	static size_t FileId;
	static size_t FolderId;
protected:
	static std::vector<void*> Stbi_image;
	static std::vector<QFAContentFile> Files; // std::vector<SEditorFileInside> EditorFile
	static std::vector<QFAContentFolder> Folders;
	static std::vector<QFAFileSystem::FolderUnit> FolderContents;

private:
	static void LoadFile(std::u32string qfaFilePth, QFAContentFile& sfile);
	static void LoadFilesInfolder(size_t folderIndex, std::u32string& textForDisplay, bool& ifTextChange);
protected:
	/*
		rework text change, throw function
	

		called during initialization
	*/
	static void LoadContent(std::u32string contentFolderPath, std::u32string& textForDisplay, bool& ifTextChange);
	
	// if SEditorFile.id == 0 file not found
	static QFAContentFile GetFile(size_t fileId);
	static QFAContentFolder GetFolder(size_t folderId);
	static void GetFolderContents(size_t folderId, std::vector<QFAFileSystem::FolderUnit>& folderContents);

	// use if user DropFiles in editor or some else
	static void AddFile(size_t folderId, QFAContentFile& contentFile);
	static void AddStbImage(void* image); // void* == stbi_uc*

	static void EndLife();
public:
	// if return nullptr mesh not found
	static QFAMeshData* GetMesh(std::u32string path);
};