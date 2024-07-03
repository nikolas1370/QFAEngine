#include "epch.h"
#define STB_IMAGE_IMPLEMENTATION 
#include "EditorFileStorage.h" // #include <stb_image.h>
#include <filesystem>
#include <unicode/unistr.h>
#include <ModelLoader.h>
#include <Tools/String.h>


size_t QFAEditorFileStorage::Id = 0;
size_t QFAEditorFileStorage::folderId = 0;
//std::vector<std::u32string> QFAEditorFileStorage::FoldersForWork;
std::vector<QFAFileSystem::FolderUnit> QFAEditorFileStorage::FolderContents;
std::vector<QFAEditorFileStorage::SEditorFileInside> QFAEditorFileStorage::EditorFile;
std::vector<SFolder> QFAEditorFileStorage::Folders;

std::vector<int> QFAEditorFileStorage::DropPath;
std::vector<stbi_uc*> QFAEditorFileStorage::Stbi_image;

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

void QFAEditorFileStorage::DropFiles(size_t folderId, int path_count, const char* paths[])
{
	/*
	
		

	

		check if paths is folder

	*/
	bool find = false;
	size_t folderIndex;
	for (size_t i = 0; i < Folders.size(); i++)
	{
		if (Folders[i].id == folderId)
		{
			find = true;
			folderIndex = i;
			break;
		}
	}

	if (!find)
		return;

	std::filesystem::path curentPath(Folders[folderIndex].path);
	curentPath.append("");// add "\\" in path

	for (size_t i = 0; i < path_count; i++)
	{
		icu::UnicodeString ucs = icu::UnicodeString::fromUTF8(paths[i]);
		UErrorCode kkl = U_ZERO_ERROR;
		DropPath.resize(ucs.length());

		int32_t strLen = ucs.toUTF32(DropPath.data(), DropPath.size(), kkl);
		std::u32string str;
		str.resize(strLen);
		for (size_t j = 0; j < ucs.length(); j++)
			str[j] = static_cast<char32_t>(DropPath[j]);


		std::filesystem::path path(str);
		std::filesystem::path ext = path.extension();
		if (std::filesystem::is_directory(path))
		{
			std::cout << "DropFile not file\n";
			continue;
		}

		bool fileExist = false;
		QFAEditorFileOnDisk ef;
		SEditorFileInside efi;
		QFAFileSystem::FolderUnit fu;
		ef.version = QFAEditorFileOnDisk::EditorFileVersion;
		if (ext == ".obj" || ext == ".fbx")
		{
			/*
				QFAEditorFileOnDisk
				QFAMeshData::SMeshInfo
				QFAMeshData::FramesData
			*/

			/*
				create fun when load model and get aiScene check if valid
				and after use aiScene like SeparateModel or solid
			*/
			QFAMeshData* md = QFAModelLoader::LoadModel(paths[i]);
			if (!md)
			{
				std::cout << "DropFile not load\n";
				continue;
			}

			ef.type = QFAEditorFileTypes::EFTMesh;
			path.replace_extension(".qfa");
			curentPath.replace_filename(path.filename());

			if (std::filesystem::exists(curentPath))
				fileExist = true;

			QFAFileSystem::WriteFile(curentPath.u32string(), &ef, sizeof(ef));

			QFAMeshData::SMeshInfo mi = md->GetMeshInfo();
			QFAFile writeFile;
			QFAFileSystem::OpenFile(curentPath.u32string(), &writeFile, false);
			QFAFileSystem::AppendFile(&writeFile, &mi, sizeof(mi));

			QFAFileSystem::AppendFile(&writeFile, md->GetFrameData(), mi.AmountData);
			QFAFileSystem::CloseFile(&writeFile);

			efi.type = QFAEditorFileTypes::EFTMesh;
			efi.fileData = md;
		}
		else if (ext == ".jpg" || ext == ".png")
		{
			/*
				QFAEditorFileOnDisk
				raw Image
			*/
			int texWidth, texHeight, texChannels;
			QFAFile rawImageFile;
			if (QFAFileSystem::LoadFile(str, &rawImageFile))
			{
				std::cout << "DropFile not load\n";
				continue;
			}

			stbi_uc* pixels = stbi_load_from_memory((stbi_uc*)rawImageFile.GetData(), rawImageFile.GetFileSize(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
			if (!pixels) // check if rawImageFile is image
			{
				std::cout << "DropFile not load\n";
				continue;
			}

			path.replace_extension(".qfa");
			curentPath.replace_filename(path.filename());

			if (std::filesystem::exists(curentPath))
				fileExist = true;

			ef.type = QFAEditorFileTypes::EFTImage;
			QFAFileSystem::WriteFile(curentPath.u32string(), &ef, sizeof(ef));

			QFAFile writeFile;
			QFAFileSystem::OpenFile(curentPath.u32string(), &writeFile, false);
			QFAFileSystem::AppendFile(&writeFile, rawImageFile.GetData(), rawImageFile.GetFileSize());
			QFAFileSystem::CloseFile(&writeFile);
			
			QFAImage::SImageCreateInfo ici;
			ici.Width = texWidth;
			ici.Height = texHeight;
			ici.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			ici.createBuffer = false;
			QFAImage* image = new QFAImage(ici);

			image->SetImage(pixels);
			Stbi_image.push_back(pixels);
			efi.type = QFAEditorFileTypes::EFTImage;
			efi.fileData = image;
		}
		else
			std::cout << "DropFile not support extension\n";

		if (fileExist)
		{
			/*
			
				if file exist need delete old file replase to new file in Folders[folderIndex].folderUnits
				and apply change in staticMesh/QFAImage
			
			*/
		}
		else
		{
			efi.path = curentPath.u32string();
			efi.id = ++Id;
			EditorFile.push_back(efi);

			fu.id = efi.id;
			fu.IsFolder = false;
			fu.name = std::filesystem::path(efi.path).filename().u32string();
			fu.path = efi.path;
			Folders[folderIndex].folderUnits.push_back(fu);
		}		
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

SEditorFile QFAEditorFileStorage::GetFile(size_t fileId)
{
	// write binary search
	for (size_t i = 0; i < EditorFile.size(); i++)
	{
		if (EditorFile[i].id == fileId)
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

QFAMeshData* QFAEditorFileStorage::GetMeshPath(std::u32string path)
{	
	std::filesystem::path meshPath(path);
	for (size_t i = 0; i < EditorFile.size(); i++)
	{
		if (std::filesystem::path(EditorFile[i].path) == path) // "Content/SomeQFAFile.qfa" == "Content\SomeQFAFile.qfa"
		{
			if (EditorFile[i].type == QFAEditorFileTypes::EFTMesh)
				return (QFAMeshData*)EditorFile[i].fileData;

			return nullptr;
		}
	}

	return nullptr;
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

	QFAEditorFileOnDisk* eFile = (QFAEditorFileOnDisk*)file->GetData();
	sfile.id = ++Id;
	if (eFile->type == QFAEditorFileTypes::EFTImage)
	{
		int texWidth, texHeight, texChannels;
		stbi_uc* pixels = stbi_load_from_memory(((const unsigned char*)eFile + sizeof(*eFile)), (int)file->GetDataSize() - sizeof(*eFile), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
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
		Stbi_image.push_back(pixels);

		sfile.type = QFAEditorFileTypes::EFTImage;
		sfile.fileData = image;
	}
	else if (eFile->type == QFAEditorFileTypes::EFTMesh)
	{
		QFAMeshData* meshData = new QFAMeshData(
			(QFAMeshData::SMeshInfo*)((char*)eFile + sizeof(*eFile)),
			((char*)eFile + sizeof(*eFile) + sizeof(QFAMeshData::SMeshInfo))
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
