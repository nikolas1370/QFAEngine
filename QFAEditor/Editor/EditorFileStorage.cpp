#include "EditorFileStorage.h"
#include <filesystem>

size_t QFAEditorFileStorage::Id = 0;
size_t QFAEditorFileStorage::folderId = 0;
//std::vector<std::u32string> QFAEditorFileStorage::FoldersForWork;
std::vector<QFAFileSystem::FolderUnit> QFAEditorFileStorage::FolderContents;
std::vector<QFAEditorFileStorage::SEditorFileInside> QFAEditorFileStorage::EditorFile;
std::vector<SFolder> QFAEditorFileStorage::Folders;


void QFAEditorFileStorage::LoadEditorFiles(std::u32string& textForDisplay, bool& ifTextChange)
{
	/*
		call one time 	
	*/
	if (Folders.size() > 0)
		stopExecute("");
	
	SFolder mainFolder;
	mainFolder.path = U"Content";
	mainFolder.id = ++folderId;
	Folders.push_back(mainFolder);
	
	for (size_t i = 0; i < Folders.size(); i++)
	{
		FolderContents.clear();
		LoadFilesInfolder(i, textForDisplay, ifTextChange);
	}
}

void QFAEditorFileStorage::LoadFilesInfolder(size_t folderIndex, std::u32string& textForDisplay, bool& ifTextChange)
{
	QFAFileSystem::GetFolderContents(Folders[folderIndex].path, FolderContents);
	for (size_t i = 0; i < FolderContents.size(); i++)
	{		
		if (FolderContents[i].IsFolder)
		{
			SFolder sfolder;
			sfolder.id = ++folderId;
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
			SEditorFileInside sfile;
			ifTextChange = true;
			textForDisplay = FolderContents[i].path;
			LoadFile(FolderContents[i].path, sfile);			
			if (sfile.id > 0)
			{
				EditorFile.push_back(sfile); 
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

void QFAEditorFileStorage::GetFolderContents(size_t folderId, std::vector<QFAFileSystem::FolderUnit>& folderContents)
{
	std::cout << "QFAEditorFileStorage::GetFolderContents " << Folders.size() << "\n";
	// write binary search
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

					for (size_t k = 0; k < EditorFile.size(); k++)
					{
						if (Folders[i].folderUnits[j].id == EditorFile[k].id)
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

SEditorFile QFAEditorFileStorage::GetFile(size_t fileIndex)
{
	// write binary search
	for (size_t i = 0; i < EditorFile.size(); i++)
	{
		if (EditorFile[i].id == fileIndex)
		{
			SEditorFile ef;
			ef.id = EditorFile[i].id;
			ef.path = EditorFile[i].path;
			ef.fileType = EditorFile[i].type;
			ef.file = EditorFile[i].fileData;
			return ef;
		}
	}

	SEditorFile ef;
	ef.id = 0;
	return ef;
}

void QFAEditorFileStorage::LoadFile(std::u32string qfaFilePAth, SEditorFileInside& sfile)
{
	sfile.path = qfaFilePAth;
	QFAFile* file = new QFAFile;
	sfile.file = file;
	if (QFAFileSystem::LoadFile(qfaFilePAth, file))
	{
		sfile.id = 0;
		return;
	}

	QFAEditorFile* eFile = (QFAEditorFile*)file->GetData();
	sfile.id = ++Id;
	if (eFile->type == QFAEditorFileTypes::EFTImage)
	{
		int texWidth, texHeight, texChannels;
		stbi_uc* pixels = stbi_load_from_memory(((const unsigned char*)eFile + sizeof(QFAEditorFile)), (int)file->GetDataSize() - sizeof(QFAEditorFile), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		if (!pixels)
		{
			--Id;
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
		stbi_image_free(pixels);

		sfile.type = QFAEditorFileTypes::EFTImage;
		sfile.fileData = image;
	}
	else if (eFile->type == QFAEditorFileTypes::EFTMesh)
	{
		MeshData* meshData = new MeshData(
			(MeshData::SMeshInfo*)((char*)eFile + sizeof(QFAEditorFile)),
			((char*)eFile + sizeof(QFAEditorFile) + sizeof(MeshData::SMeshInfo))
		);

		sfile.type = QFAEditorFileTypes::EFTMesh;
		sfile.fileData = meshData;
	}
	else
	{
		--Id;
		sfile.id = 0;
	}
	
}
