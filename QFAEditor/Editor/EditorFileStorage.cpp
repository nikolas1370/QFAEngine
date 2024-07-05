#include "epch.h"
#define STB_IMAGE_IMPLEMENTATION 
#include <stb_image.h>
#include "EditorFileStorage.h" 
#include <filesystem>
#include <unicode/unistr.h>
#include <ModelLoader.h>
#include <Tools/String.h>

std::vector<int> QFAEditorFileStorage::DropPath;
void QFAEditorFileStorage::DropFiles(size_t folderId, int path_count, const char* paths[])
{
	/*





		check if paths is folder

	*/
	QFAContentFolder cf = QFAContentManager::GetFolder(folderId);
	if (!cf.id)
		return;
	
	std::filesystem::path curentPath(cf.path);
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
		QFAFileOnDisk fd;
		QFAContentFile cf; 
		fd.version = QFAFileOnDisk::EditorFileVersion;
		if (ext == ".obj" || ext == ".fbx")
		{
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

			fd.type = QFAFileTypes::EFTMesh;
			path.replace_extension(".qfa");
			curentPath.replace_filename(path.filename());

			if (std::filesystem::exists(curentPath))
				fileExist = true;

			QFAFileSystem::WriteFile(curentPath.u32string(), &fd, sizeof(fd));

			QFAMeshData::SMeshInfo mi = md->GetMeshInfo();
			QFAFile writeFile;
			QFAFileSystem::OpenFile(curentPath.u32string(), &writeFile, false);
			QFAFileSystem::AppendFile(&writeFile, &mi, sizeof(mi));

			QFAFileSystem::AppendFile(&writeFile, md->GetFrameData(), mi.AmountData);
			QFAFileSystem::CloseFile(&writeFile);

			cf.fileType = QFAFileTypes::EFTMesh;
			cf.file = md;
		}
		else if (ext == ".jpg" || ext == ".png")
		{
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

			fd.type = QFAFileTypes::EFTImage;
			QFAFileSystem::WriteFile(curentPath.u32string(), &fd, sizeof(fd));

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
			cf.fileType = QFAFileTypes::EFTImage;
			cf.file = image;
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
			cf.path = curentPath.u32string();
			AddFile(folderId , cf);
		}
	}
}
