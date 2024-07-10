#include "pch.h"
#include "FileSystem.h"
#include <EngineStuff/VulkanSuff.h>
#include <filesystem>

std::vector<void*> QFAFile::Datas;

void QFAFile::EndLife()
{
	for (size_t i = 0; i < Datas.size(); i++)
		delete[] Datas[i];
}

void QFAFile::Delete()
{
	if (file)
		delete[] file;

	for (size_t i = 0; i < Datas.size(); i++)
	{
		if (Datas[i] == file)
		{
			Datas.erase(Datas.begin() + i);
			file = nullptr;
			return;
		}
	}
}

void QFAFile::DisableAutoDeleteFileData()
{
	AutoDeleteFileData = false;
	if(file)
		Datas.push_back(file);
}

void QFAFile::ReleaseData(void* fileData)
{
	for (size_t i = 0; i < Datas.size(); i++)
	{
		if (Datas[i] == fileData)
		{
			delete[] Datas[i];
			Datas.erase(Datas.begin() + i);
			return;
		}
	}
}

std::string QFAFile::GetPathString()
{
	return std::filesystem::path(path).string();
}

inline std::wstring QFAFile::GetPathWString()
{
	return std::filesystem::path(path).wstring();
}

QFAFile::Result QFAFileSystem::LoadFile(std::u32string path, QFAFile* file)
{	
	if (!file)
		stopExecute("QFAFileSystem::LoadFile file == nullptr");

	if (!std::filesystem::exists(path))
	{
		file->res = QFAFile::Result::FileNotExists;
		return QFAFile::Result::FileNotExists;
	}
	
	file->openFileIN.open(path, std::ios::in | std::ios::binary | std::ios::ate);
	if (!file->openFileIN.is_open())
	{
		file->res = QFAFile::Result::FileNotExists;
		return QFAFile::Result::FileNotExists;
	}
	
	file->size = file->openFileIN.tellg();// Returns the position of the current character in the input stream.	
	file->file = new char[file->size];
	file->openFileIN.seekg(0, std::ios::beg);
	file->openFileIN.read((char*)file->file, file->size);
	file->openFileIN.close();
	file->path = path;
	return QFAFile::Result::Success;
}

QFAFile::Result QFAFileSystem::WriteFile(std::u32string path, void* data, std::streamsize size)
{	
	if (!data)
		stopExecute("QFAFileSystem::WriteFile data == nullptr");
	
	std::filesystem::path kioPAth = path;	
	if (kioPAth.parent_path().string().size() > 0)
		std::filesystem::create_directories(kioPAth.parent_path());

	std::filesystem::path filePath = path;
	std::ofstream openFile(filePath, std::ofstream::out | std::ofstream::trunc | std::ofstream::binary );
	if (!openFile.is_open())
		return QFAFile::Result::CannotOpenFile;
	
	openFile.write((const char*)data, size);
	openFile.close();
	return QFAFile::Result::Success;
}


QFAFile::Result QFAFileSystem::OpenFile(std::u32string path, QFAFile* file, bool createFile)
{
	if (!file)
		stopExecute("QFAFileSystem::OpenFile file == nullptr");

	if (!std::filesystem::exists(path) )
	{
		if (createFile)
		{
			std::filesystem::path kioPAth = path;						
			if(kioPAth.parent_path().string().size() > 0)
				std::filesystem::create_directories(kioPAth.parent_path());
		}
		else
		{
			file->res = QFAFile::Result::FileNotExists;
			return QFAFile::Result::FileNotExists;
		}		
	}
	
	file->openFileOUT.open(path.c_str(), std::ofstream::out | std::ofstream::binary | std::ofstream::app);
	if (!file->openFileOUT.is_open())
	{		
		file->res = QFAFile::Result::CannotOpenFile;
		return QFAFile::Result::CannotOpenFile;
	}	

	

	return QFAFile::Result::Success;	
}

void QFAFileSystem::CloseFile(QFAFile* file)
{
	if (!file)
		return;
	
	file->openFileOUT.close();
}

QFAFile::Result QFAFileSystem::AppendFile(QFAFile* file, void* data, std::streamsize size)
{
	if (!data && !file)
		stopExecute("QFAFileSystem::AppendFile data == nullptr");

	if (!file->openFileOUT.is_open())
		return QFAFile::FileNotOpen;
	
	file->openFileIN.seekg(0, std::ios_base::end);
	file->openFileOUT.write((const char*)data, size);
	return QFAFile::Result::Success;
}

bool QFAFileSystem::GetFileList(std::u32string folderPath, std::vector<std::u32string>& filePaths)
{
	if (std::filesystem::exists(folderPath) && std::filesystem::is_directory(folderPath))
	{
		for (const auto& entry : std::filesystem::directory_iterator(folderPath))
			filePaths.push_back(entry.path().u32string());

		return true;
	}
	else
		return false;		
}

size_t QFAFileSystem::LastWriteTime(std::u32string filePath)
{
	if (std::filesystem::exists(filePath) && !std::filesystem::is_directory(filePath))
		return (size_t)std::filesystem::last_write_time(filePath).time_since_epoch().count();
	else	
		return 0;	
}

std::u32string QFAFileSystem::GetExtension(std::u32string filePath)
{
	std::filesystem::path path = filePath;	
	return path.extension().u32string();
}

bool QFAFileSystem::exists(std::u32string filePath)
{	
	return std::filesystem::exists(filePath);
}

void QFAFileSystem::CreateFolders(std::u32string folderPath)
{
	std::filesystem::create_directories(folderPath);
}

bool QFAFileSystem::GetFolderContents(std::u32string path, std::vector<FolderUnit>& folderContents)
{
	if (!std::filesystem::exists(path))
		return false;

	for (const auto& unit : std::filesystem::directory_iterator(path))
		folderContents.push_back(FolderUnit{ unit.path().u32string() , unit.path().filename().u32string() , 0, unit.is_directory() });

	if (folderContents.size() <= 1)
		return true;


	for (size_t i = 0; i < folderContents.size() - 1; i++)
	{
		for (size_t j = 0; j < folderContents.size() - 1; j++)
		{
			if (!folderContents[j].IsFolder && folderContents[j + 1].IsFolder)
			{
				FolderUnit fu = folderContents[j];
				folderContents[j] = folderContents[j + 1];
				folderContents[j + 1] = fu;
			}
		}
	}

	return true;
}

