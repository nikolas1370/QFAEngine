#include "pch.h"
#include "ContentManager.h"
#include <EngineStuff/Image.h>
#include <filesystem>
#include <Object/ActorComponent/SceneComponent/Mesh/MeshBase.h>
#include <UI/UIImage.h>
#include <stb_image.h>
#include <Tools/String.h>
#include <Object/World/Level.h>

std::vector<void*> QFAContentManager::Stbi_image; // <stbi_uc*>
std::vector<QFAContentManager::QFAContentFile> QFAContentManager::Files;
std::vector<QFAContentManager::QFAContentFolder> QFAContentManager::Folders;
std::vector<QFAFileSystem::FolderUnit> QFAContentManager::FolderContents; 
std::function<void(std::u32string)> QFAContentManager::ChangeTextFun;

void QFAContentManager::LoadContent(std::u32string contentFolderPath, std::function<void(std::u32string)> changeText)
{
	ChangeTextFun = changeText;
	if (Folders.size() > 0)
		stopExecute("");

	if (!std::filesystem::exists(contentFolderPath) || !std::filesystem::is_directory(contentFolderPath))
		stopExecute("Content folder not exist or not folder");

	QFAContentFolder mainFolder;
	Folders.push_back(mainFolder);		// zero id(index)
	Files.push_back(QFAContentFile{});	// zero id(index)
	
	mainFolder.path = contentFolderPath;
	mainFolder.Id = Folders.size();
	Folders.push_back(mainFolder);

	for (size_t i = 0; i < Folders.size(); i++)
		LoadFilesInfolder(i);
}

void QFAContentManager::LoadFilesInfolder(size_t folderIndex)
{
	FolderContents.clear();
	QFAFileSystem::GetFolderContents(Folders[folderIndex].path, FolderContents);
	for (size_t i = 0; i < FolderContents.size(); i++)
	{
		if (FolderContents[i].IsFolder)
		{
			QFAContentFolder sfolder;
			sfolder.Id = Folders.size();
			sfolder.path = FolderContents[i].path;
			Folders.push_back(sfolder);
			Folders[folderIndex].folderUnits.push_back(EditorFolderUnit{ sfolder.Id , true });
		}
		else if (std::filesystem::path(FolderContents[i].name).extension() == U".qfa")
		{
			QFAContentFile sfile;
			ChangeTextFun(FolderContents[i].path);
			if (LoadFile(FolderContents[i].path, sfile))
			{
				sfile.Id = Files.size();
				if (sfile.fileType == QFAFileTypes::EFTMesh)
					((QFAMeshData*)sfile.file)->FIleid = sfile.Id;

				Files.push_back(sfile);
				Folders[folderIndex].folderUnits.push_back(EditorFolderUnit{ sfile.Id , false});
			}
		}
	}
}

QFAContentManager::QFAContentFile QFAContentManager::WriteMesh(std::u32string filePath, QFAMeshData* mesh)
{
	QFAContentFile cf;		
	QFAFileOnDisk fd(QFAFileTypes::EFTMesh, MeshFileVersion);

	QFAFileSystem::WriteFile(filePath, &fd, sizeof(fd));

	QFAMeshData::SMeshInfo mi = mesh->GetMeshInfo();
	QFAFile writeFile;
	QFAFileSystem::OpenFile(filePath, &writeFile, false);
	QFAFileSystem::AppendFile(&writeFile, &mi, sizeof(mi));

	QFAFileSystem::AppendFile(&writeFile, mesh->GetFrameData(), mi.AmountData);
	QFAFileSystem::CloseFile(&writeFile);

	cf.fileType = QFAFileTypes::EFTMesh;
	cf.file = mesh;
	cf.path = filePath;
	return cf;
}

QFAContentManager::QFAContentFile QFAContentManager::WriteImage(std::u32string filePath, QFAImage* image)
{
	QFAContentFile cf;
	QFAFileOnDisk fd(QFAFileTypes::EFTImage, ImageFileVersion);
	
	QFAFileSystem::WriteFile(filePath, &fd, sizeof(fd));

	QFAFile writeFile;
	QFAFileSystem::OpenFile(filePath, &writeFile, false);
	QFAFileSystem::AppendFile(&writeFile, image->File->GetData(), image->File->GetFileSize());
	QFAFileSystem::CloseFile(&writeFile);
	
	cf.fileType = QFAFileTypes::EFTImage;
	cf.file = image;
	cf.path = filePath;
	return cf;
}

QFAContentManager::QFAContentFile QFAContentManager::WriteLevel(std::u32string filePath, QFALevel* level)
{// file write on disk in QFALevel
	QFAContentFile cf;
	cf.fileType = QFAFileTypes::EFTLevel;
	cf.file = level;
	cf.path = filePath;
	return cf;
}

bool QFAContentManager::LoadFile(std::u32string qfaFilePAth, QFAContentFile& sfile)
{
	sfile.path = qfaFilePAth;
	QFAFile* file = new QFAFile;
	if (QFAFileSystem::LoadFile(qfaFilePAth, file))
		return false;

	QFAFileOnDisk* eFile = (QFAFileOnDisk*)file->GetData();
	switch (eFile->type)
	{
		case QFAFileTypes::EFTImage: return LoadFileImage(file, sfile);
		case QFAFileTypes::EFTMesh:  return LoadFileMesh(file, sfile);
		case QFAFileTypes::EFTLevel: return LoadFileLevel(file, sfile);
		default:					 return false;
	}
}

bool QFAContentManager::LoadFileImage(QFAFile* file, QFAContentFile& sfile)
{
	QFAFileOnDisk* eFile = (QFAFileOnDisk*)file->GetData();
	int texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load_from_memory(((const unsigned char*)eFile + sizeof(*eFile)), (int)file->GetDataSize() - sizeof(*eFile), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	if (!pixels)
		return false;

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
	delete file;
	return true;
}

bool QFAContentManager::LoadFileMesh(QFAFile* file, QFAContentFile& sfile)
{
	QFAFileOnDisk* eFile = (QFAFileOnDisk*)file->GetData();
	QFAMeshData* meshData = new QFAMeshData(
		(QFAMeshData::SMeshInfo*)((char*)eFile + sizeof(*eFile)),
		((char*)eFile + sizeof(*eFile) + sizeof(QFAMeshData::SMeshInfo)), file);

	sfile.fileType = QFAFileTypes::EFTMesh;
	sfile.file = meshData;
	return true;
}

bool QFAContentManager::LoadFileLevel(QFAFile* file, QFAContentFile& sfile)
{	
	sfile.fileType = QFAFileTypes::EFTLevel;
	sfile.file = new QFALevel(sfile.path); // level load data if call QFALevel::GetWorld
	file->Delete();
	return true;
}


QFAContentManager::QFAContentFile NotFoundContentFile;
QFAContentManager::QFAContentFile& QFAContentManager::GetFile(int fileId) 
{
	if(fileId >= 0 && fileId < Files.size())
		return Files[fileId];	

	NotFoundContentFile.Id = -1;
	return NotFoundContentFile;
}

std::u32string QFAContentManager::GetFolderPath(size_t folderId)
{	
	for (size_t i = 0; i < Folders.size(); i++)
		if (Folders[i].Id == folderId)
			return Folders[i].path;
	
	return std::u32string();
}

void QFAContentManager::GetFolderContents(size_t folderId, std::vector<QFAFileSystem::FolderUnit>& folderContents)
{
	QFAContentFolder& Folder = Folders[folderId];
	QFAFileSystem::FolderUnit fc;
	for (size_t i = 0; i < Folder.folderUnits.size(); i++)
	{		
		fc.id = Folder.folderUnits[i].Id;
		if (Folder.folderUnits[i].IsFolder)
		{			
			fc.IsFolder = true;
			fc.name = std::filesystem::path(Folders[fc.id].path).filename().u32string();
			fc.path = Folders[fc.id].path;						
		}
		else
		{
			fc.IsFolder = false;
			fc.name = std::filesystem::path(Files[fc.id].path).filename().u32string();
			fc.path = Files[fc.id].path;
		}

		folderContents.push_back(fc);
	}
}

QFAContentManager::QFAContentFolder& QFAContentManager::GetFolder(std::u32string path)
{
	for (size_t i = 0; i < Folders.size(); i++)
		if (Folders[i].path == path)
			return Folders[i];
}

void QFAContentManager::AddFile(size_t folderId, std::u32string filePath, QFAFileTypes fyleType, void* file)
{
	QFAContentFile contentFile;
	if (fyleType == QFAFileTypes::EFTImage)
		contentFile = WriteImage(filePath, (QFAImage*)file);
	else if (fyleType == QFAFileTypes::EFTMesh)
		contentFile = WriteMesh(filePath, (QFAMeshData*)file);
	else if (fyleType == QFAFileTypes::EFTLevel)
		contentFile = WriteLevel(filePath, (QFALevel*)file);

	for (size_t i = 0; i < Folders[folderId].folderUnits.size(); i++)
	{		
		if (Files[Folders[folderId].folderUnits[i].Id].path == filePath)
		{
			QFAContentFile& confile = Files[Folders[folderId].folderUnits[i].Id];
			switch (fyleType)
			{
				case QFAContentManager::EFTUndefined:
					break;
				case QFAContentManager::EFTMesh:
					((QFAMeshData*)confile.file)->UpdateMeshData((QFAMeshData*)file);
					((QFAMeshData*)confile.file)->FIleid = confile.Id;
					confile.file = file;
					break;
				case QFAContentManager::EFTImage:
					((QFAImage*)confile.file)->UpdateImage((QFAImage*)file);
					confile.file = file;
					break;
				case QFAContentManager::EFTLevel: // for level confile.file not change
					break;
				default:
					break;
			}			

			return;
		}
	}

	contentFile.Id = Files.size(); // FIleid
	if (fyleType == QFAFileTypes::EFTMesh)
		((QFAMeshData*)contentFile.file)->FIleid = contentFile.Id;

	Files.push_back(contentFile);
	Folders[folderId].folderUnits.push_back(EditorFolderUnit{ contentFile.Id, false });
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

void* QFAContentManager::GetFileForUser(std::u32string path, QFAFileTypes ft)
{
	std::filesystem::path meshPath(path);
	for (size_t i = 0; i < Files.size(); i++)
	{
		if (std::filesystem::path(Files[i].path) == path) // "Content/SomeQFAFile.qfa" == "Content\SomeQFAFile.qfa"
		{
			if (Files[i].fileType == ft)
				return Files[i].file;

			return nullptr;
		}
	}

	return nullptr;
}