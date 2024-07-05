#include "pch.h"
#include "ContentManager.h"
#include <Render/Image.h>
#include <filesystem>
#include <Object/ActorComponent/SceneComponent/Mesh/MeshBase.h>
#include <stb_image.h>
std::vector<void*> QFAContentManager::Stbi_image; // <stbi_uc*>
size_t QFAContentManager::FileId; 
size_t QFAContentManager::FolderId;
std::vector<QFAContentManager::QFAContentFile> QFAContentManager::Files;
std::vector<QFAContentManager::QFAContentFolder> QFAContentManager::Folders;
std::vector<QFAFileSystem::FolderUnit> QFAContentManager::FolderContents; 

void QFAContentManager::LoadContent(std::u32string contentFolderPath, std::u32string& textForDisplay, bool& ifTextChange)
{
	if (Folders.size() > 0)
		stopExecute("");

	if (!std::filesystem::exists(contentFolderPath) || !std::filesystem::is_directory(contentFolderPath))
		stopExecute("Content folder not exist or not folder");

	QFAContentFolder mainFolder;
	mainFolder.path = contentFolderPath;
	mainFolder.id = ++FolderId;
	Folders.push_back(mainFolder);

	for (size_t i = 0; i < Folders.size(); i++)
	{
		FolderContents.clear();
		LoadFilesInfolder(i, textForDisplay, ifTextChange);
	}
}

void QFAContentManager::LoadFilesInfolder(size_t folderIndex, std::u32string& textForDisplay, bool& ifTextChange)
{
	QFAFileSystem::GetFolderContents(Folders[folderIndex].path, FolderContents);
	for (size_t i = 0; i < FolderContents.size(); i++)
	{
		if (FolderContents[i].IsFolder)
		{
			QFAContentFolder sfolder;
			sfolder.id = ++FolderId;
			sfolder.path = FolderContents[i].path;
			Folders.push_back(sfolder);

			QFAFileSystem::FolderUnit fu;
			fu.id = sfolder.id;
			fu.IsFolder = true;
			fu.path = sfolder.path;
			fu.name = std::filesystem::path(sfolder.path).filename().u32string();
			Folders[folderIndex].folderUnits.push_back(fu);
		}
		else if (std::filesystem::path(FolderContents[i].name).extension() == U".qfa")
		{
			QFAContentFile sfile;
			ifTextChange = true;
			textForDisplay = FolderContents[i].path;
			LoadFile(FolderContents[i].path, sfile);
			if (sfile.id > 0)
			{
				Files.push_back(sfile);
				QFAFileSystem::FolderUnit fu;
				fu.id = sfile.id;
				fu.IsFolder = false;
				fu.name = std::filesystem::path(sfile.path).filename().u32string();
				fu.path = sfile.path;
				Folders[folderIndex].folderUnits.push_back(fu);
			}
		}
	}
}

void QFAContentManager::LoadFile(std::u32string qfaFilePAth, QFAContentFile& sfile)
{
	sfile.path = qfaFilePAth;
	QFAFile* file = new QFAFile;
	sfile.file = file;
	if (QFAFileSystem::LoadFile(qfaFilePAth, file))
	{
		sfile.id = 0;
		return;
	}

	QFAFileOnDisk* eFile = (QFAFileOnDisk*)file->GetData();
	sfile.id = ++FileId;
	if (eFile->type == QFAFileTypes::EFTImage)
	{
		int texWidth, texHeight, texChannels;
		stbi_uc* pixels = stbi_load_from_memory(((const unsigned char*)eFile + sizeof(*eFile)), (int)file->GetDataSize() - sizeof(*eFile), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		if (!pixels)
		{
			--FileId;
			sfile.id = 0;
			return;
		}

		QFAImage::SImageCreateInfo ici;
		ici.Width = texWidth;
		ici.Height = texHeight;
		ici.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		ici.createBuffer = false;
		QFAImage* image = new QFAImage(ici);

		image->SetImage(pixels);
		Stbi_image.push_back(pixels);

		sfile.fileType = QFAFileTypes::EFTImage;
		sfile.file = image;
	}
	else if (eFile->type == QFAFileTypes::EFTMesh)
	{
		QFAMeshData* meshData = new QFAMeshData(
			(QFAMeshData::SMeshInfo*)((char*)eFile + sizeof(*eFile)),
			((char*)eFile + sizeof(*eFile) + sizeof(QFAMeshData::SMeshInfo))
		);

		sfile.fileType = QFAFileTypes::EFTMesh;
		sfile.file = meshData;
	}
	else
	{
		--FileId;
		sfile.id = 0;
	}
}

QFAContentManager::QFAContentFile QFAContentManager::GetFile(size_t fileId) 
{
	// write binary search std::binary_search
	for (size_t i = 0; i < Files.size(); i++)
	{
		if (Files[i].id == fileId)
		{
			QFAContentFile ef;
			ef.id = Files[i].id;
			ef.path = Files[i].path;
			ef.fileType = Files[i].fileType;
			ef.file = Files[i].file;
			return ef;
		}
	}

	QFAContentFile ef;
	ef.id = 0;
	return ef;
}

QFAContentManager::QFAContentFolder QFAContentManager::GetFolder(size_t folderId)
{
	// write binary search std::binary_search
	for (size_t i = 0; i < Folders.size(); i++)
		if (Folders[i].id == folderId)
			return Folders[i];

	QFAContentFolder cf;
	cf.id = 0;
	return QFAContentFolder();
}


void QFAContentManager::GetFolderContents(size_t folderId, std::vector<QFAFileSystem::FolderUnit>& folderContents)
{
	// write binary search std::binary_search
	for (size_t i = 0; i < Folders.size(); i++)
	{
		if (Folders[i].id == folderId)
		{
			for (size_t j = 0; j < Folders[i].folderUnits.size(); j++)
			{// first add folder
				if (Folders[i].folderUnits[j].IsFolder)
				{
					for (size_t k = 0; k < Folders.size(); k++)
					{
						if (Folders[i].folderUnits[j].id == Folders[k].id)
						{
							folderContents.push_back(Folders[i].folderUnits[j]);
							break;
						}
					}
				}
			}

			for (size_t j = 0; j < Folders[i].folderUnits.size(); j++)
			{ // second add files
				if (!Folders[i].folderUnits[j].IsFolder)
				{
					for (size_t k = 0; k < Files.size(); k++)
					{
						if (Folders[i].folderUnits[j].id == Files[k].id)
						{
							folderContents.push_back(Folders[i].folderUnits[j]);
							break;
						}
					}
				}
			}

			return;
		}
	}
}

void QFAContentManager::AddFile(size_t folderId, QFAContentFile& contentFile)
{
	contentFile.id = ++FileId;
	Files.push_back(contentFile);

	QFAFileSystem::FolderUnit fu;
	fu.id = contentFile.id;
	fu.IsFolder = false;
	fu.name = std::filesystem::path(contentFile.path).filename().u32string();
	fu.path = contentFile.path;
	// write binary search std::binary_search
	for (size_t i = 0; i < Folders.size(); i++)
	{
		if (Folders[i].id == folderId)
		{
			Folders[i].folderUnits.push_back(fu);
			return;
		}
	}
}

void QFAContentManager::AddStbImage(void* image)
{
	Stbi_image.push_back(image);
}

void QFAContentManager::EndLife()
{
	/*
delete files

*/
	for (size_t i = 0; i < Stbi_image.size(); i++)
		stbi_image_free(Stbi_image[i]);
}

QFAMeshData* QFAContentManager::GetMesh(std::u32string path) 
{
	std::filesystem::path meshPath(path);
	for (size_t i = 0; i < Files.size(); i++)
	{
		if (std::filesystem::path(Files[i].path) == path) // "Content/SomeQFAFile.qfa" == "Content\SomeQFAFile.qfa"
		{
			if (Files[i].fileType == QFAFileTypes::EFTMesh)
				return (QFAMeshData*)Files[i].file;

			return nullptr;
		}
	}

	return nullptr;
}