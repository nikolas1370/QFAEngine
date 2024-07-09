#pragma once
#include <string>
#include <Tools/Stuff.h>
#include <Tools/File/FileSystem.h>
class QFAMeshData;
class QFAOverlord;
class QFAImage;

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

	struct QFAFileOnDisk
	{		
		const char signature[10] = "QFAEngine";
		QFAFileTypes type;
		unsigned short version ;
		QFAFileOnDisk() {}
		QFAFileOnDisk(QFAFileTypes fileType, unsigned int fileVersion)
		{
			type = fileType;
			version = fileVersion;
		}
	};

	struct EditorFolderUnit
	{
		size_t Id = 0;
		bool IsFolder;
	};

    struct QFAContentFolder 
    {
        std::u32string path;
        std::vector<EditorFolderUnit> folderUnits; // before std::vector<QFAFileSystem::FolderUnit> folderUnits;
        size_t Id = 0;		// Id == index in Files
    };

	struct SWriteImage
	{
		QFAFile* rawImageFile;
		void* pixels; // stbi_uc* 
		int texWidth;
		int texHeight;
	};

public:
	struct QFAContentFile 
	{
		std::u32string path;
		void* file = nullptr; // cast data to (QFAImage, QFAMeshData, etc)
		size_t Id = 0;	// Id == index in Files
		QFAFileTypes fileType;
	};

protected:
	static const unsigned short MeshFileVersion = 1;
	static const unsigned short ImageFileVersion = 1;
	static std::vector<void*> Stbi_image; // stbi_uc*
	static std::vector<QFAContentFile> Files;  // QFAContentFile.Id == index 
	static std::vector<QFAContentFolder> Folders; // QFAContentFolder.Id == index 
	static std::vector<QFAFileSystem::FolderUnit> FolderContents; // use for QFAFileSystem::GetFolderContents 

private:
	static bool LoadFile(std::u32string qfaFilePth, QFAContentFile& sfile);
	static void LoadFilesInfolder(size_t folderIndex, std::u32string& textForDisplay, bool& ifTextChange);
	static QFAContentFile WriteMesh(std::u32string filePath, QFAMeshData* mesh);
	static QFAContentFile WriteImage(std::u32string filePath, QFAImage* image);

protected:
	/*
		rework text change, throw function
	

		called during initialization
	*/
	static void LoadContent(std::u32string contentFolderPath, std::u32string& textForDisplay, bool& ifTextChange);
	
	
	// if SEditorFile.id == 0 file not found
	static QFAContentFile& GetFile(size_t fileId);
	
	static std::u32string GetFolderPath(size_t folderId);
	static void GetFolderContents(size_t folderId, std::vector<QFAFileSystem::FolderUnit>& folderContents);

	/*
		file update if exist
		
		use if user DropFiles in editor or some else		
	*/
	static void AddFile(size_t folderId, std::u32string filePath, QFAFileTypes fyleType, void* file);

	static void AddStbImage(void* image); // void* == stbi_uc*

	static void EndLife();

	static void* GetFileForUser(std::u32string path, QFAFileTypes ft);
public:
	// if return nullptr mesh not found
	static inline QFAMeshData* GetMesh(std::u32string path)
	{
		return (QFAMeshData*)GetFileForUser(path, QFAFileTypes::EFTMesh);
	}
	// if return nullptr image not found
	static inline QFAImage* GetImage(std::u32string path)
	{
		return (QFAImage*)GetFileForUser(path, QFAFileTypes::EFTImage);
	}
};