#pragma once
#include <string>
#include <fstream>
#include <vector>

class QFAFileSystem;
class QFAOverlord;
class QFAFile
{
	friend QFAOverlord;
	friend QFAFileSystem;
	std::u32string path;
	void* file = nullptr;
	std::ofstream openFileOUT;
	std::ifstream openFileIN;
	size_t size = 0;	
	bool AutoDeleteFileData = true;

	static std::vector<void*> Datas;

	static void EndLife();
public:
	enum Result
	{
		Success = 0,
		FileNotExists,
		CannotOpenFile,
		FileNotOpen
	};

	QFAFile() {}
	~QFAFile()
	{
		openFileOUT.close();
		if (AutoDeleteFileData && file)
			delete[] file;

		file = nullptr;
	}

	/*
		if the data is not need use QFAFile::ReleaseData
	*/
	void DisableAutoDeleteFileData();

	static void ReleaseData(void* fileData);


	inline void* GetData()
	{
		return file;
	}

	inline size_t GetDataSize()
	{
		return size;
	}

	inline QFAFile::Result GetStatus()
	{
		return res;
	}

	inline size_t GetFileSize()
	{
		return size;
	}

	inline std::u32string GetPath()
	{
		
		return path;
	}

	std::string GetPathString();

	inline std::wstring GetPathWString();
private:
	Result res;
};



// FindFirstChangeNotification https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-findfirstchangenotificationa?redirectedfrom=MSDN
class QFAFileSystem
{
public:
	static QFAFile::Result LoadFile(std::u32string path, QFAFile* file);
	/*
		remove all from file before write
		folder/file be create if not exist
	*/
	static QFAFile::Result WriteFile(std::u32string path, void* data, std::streamsize size);

	/*
	* Open File for AppendFile
	* 
		if createFile == true folder/file be create if does not exist
	*/
	static QFAFile::Result OpenFile(std::u32string path, QFAFile* file, bool createFile);
	static void CloseFile(QFAFile* file);
	
	/*
		
		file should be open with OpenFile()
	*/
	static QFAFile::Result AppendFile(QFAFile* file, void* data, std::streamsize size);

	/*
		return false if folder don't exist or it's path to file
	*/
	static bool GetFileList(std::u32string folderPath, std::vector<std::u32string>& filePaths);

	/*
		return 0 if file don't exist or it's path to folder
	*/
	static size_t LastWriteTime(std::u32string filePath);

	static std::u32string GetExtension(std::u32string filePath);

	static bool exists(std::u32string filePath);

	static void CreateFolders(std::u32string folderPath);
	
private:

};