#include "epch.h"
#define STB_IMAGE_IMPLEMENTATION 
#include <stb_image.h>
#include "EditorFileStorage.h" 
#include <filesystem>
#include <unicode/unistr.h>
#include <ModelLoader.h>
#include <Tools/String.h>
#include <EngineClassesInterface.h>

std::vector<int> QFAEditorFileStorage::DropPath;
void QFAEditorFileStorage::DropFiles(size_t folderId, int path_count, const char* paths[])
{
	/*





		check if paths is folder

	*/
	
	std::u32string folderPath = QFAContentManager::GetFolderPath(folderId);
	if (!folderPath.size())
		return;
	
	std::filesystem::path curentPath(folderPath);
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
		path.replace_extension(".qfa");
		curentPath.replace_filename(path.filename());		
		if (std::filesystem::is_directory(path))
		{
			std::cout << "DropFile not file\n";
			continue;
		}

		if (ext == ".obj" || ext == ".fbx")
		{
			/*
				create fun when load model and get aiScene check if valid
				and after use aiScene like SeparateModel or solid
			*/			
			if (QFAMeshData* md = QFAModelLoader::LoadModel(paths[i]))
				AddFile(folderId, curentPath.u32string(), QFAFileTypes::EFTMesh, md);
			else
			{
				std::cout << "DropFile not load\n";
				continue;
			}
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
			
			if (stbi_uc* pixels = stbi_load_from_memory((stbi_uc*)rawImageFile.GetData(),
				rawImageFile.GetFileSize(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha)) // check if rawImageFile is image
			{
				QFAImage::SImageCreateInfo ici;
				ici.Width = texWidth;
				ici.Height = texHeight;
				ici.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				ici.createBuffer = false;
				
				QFAImage* image = QFAEditorImage::CreateImage(ici);
				((QFAEditorImage*)image)->SetImage(pixels);
				((QFAEditorImage*)image)->File = &rawImageFile; 
				AddFile(folderId, curentPath.u32string(), QFAFileTypes::EFTImage, image);
				stbi_image_free(pixels);
			}
			else
			{
				std::cout << "DropFile not load\n";
				continue;
			}
		}
		else
			std::cout << "DropFile not support extension\n";
	}
}
