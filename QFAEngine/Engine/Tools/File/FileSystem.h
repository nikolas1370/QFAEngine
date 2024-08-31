#pragma once
#include <Tools/Stuff.h>
#include <string>
#include <fstream>
#include <vector>

class QFAFileSystem;
class QFAOverlord;

enum QFileResult
{
	QFRSuccess = 0,
	QFRFileNotExists,
	QFRCannotOpenFile,
	QFRFileNotOpen,
	QFREndFileReached
};

class QFAEXPORT QFAFile
{
	friend QFAOverlord;
	friend QFAFileSystem;

public:
	enum Result
	{
		Success = 0,
		FileNotExists,
		CannotOpenFile,
		FileNotOpen
	};

private:
	static std::vector<void*> Datas;

	std::u32string path;
	void* file = nullptr;
	std::ofstream openFileOUT;
	std::ifstream openFileIN;
	size_t size = 0;	
	bool AutoDeleteFileData = true;
	Result res;

	static void EndLife();

public:
	static void ReleaseData(void* fileData);

	~QFAFile()
	{
		openFileOUT.close();
		if (AutoDeleteFileData && file)
			delete[] file;

		file = nullptr;
	}

	void Delete();

	/*
		if the data is not need use QFAFile::ReleaseData
	*/
	void DisableAutoDeleteFileData();


	inline void* GetData()
	{
		return file;
	}

	inline size_t GetDataSize()
	{
		return size;
	}

	inline size_t GetFileSize()
	{
		return size;
	}

	inline QFAFile::Result GetStatus()
	{
		return res;
	}

	inline std::u32string GetPath()
	{		
		return path;
	}

	std::string GetPathString();

	inline std::wstring GetPathWString();
};

class QFAEXPORT QFAFileReadStream
{
	std::u32string Path;
	std::ifstream OpenFileIN; // QFileResult
	size_t FileSize = 0;

public:
	QFAFileReadStream();
	~QFAFileReadStream();
	QFileResult Open(const std::u32string& filePAth);
	inline void Close()
	{
		OpenFileIN.close();
	}
	inline bool IsOpen() 
	{ 
		return OpenFileIN.is_open();
	}
	/*
		if notReadFrom > 0 file not be read after notReadFrom bytes
			example
			SetFilePointer(10);
			Read(100, somePtr , amountByteWasRead, 80);
			in somePtr be writed only 70 bytes
	*/
	QFileResult Read(const size_t count, const void* data, size_t& amountByteWasRead, const size_t notReadFrom = 0);
	inline void SetFilePointer(const size_t pos)
	{
		if (OpenFileIN.is_open())			
			OpenFileIN.seekg(pos);
	}

	inline void SetFilePointerToEnd()
	{
		if (OpenFileIN.is_open())
			OpenFileIN.seekg(0, std::ios::end);
	}

	// 	file->openFileIN.seekg(0, std::ios::beg);

	inline size_t GetFilePointer()
	{
		return (OpenFileIN.is_open() ? (size_t)OpenFileIN.tellg(): 0);
	}

	inline size_t GetFileSize()
	{
		return FileSize;
	}
};

// FindFirstChangeNotification https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-findfirstchangenotificationa?redirectedfrom=MSDN
class QFAEXPORT QFAFileSystem
{
public:
	struct FolderUnit
	{
		std::u32string path;
		std::u32string name;
		size_t id; // not for QFAFileSystem
		bool IsFolder;
	};

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

	static bool GetFolderContents(std::u32string path, std::vector<FolderUnit>& folderContents);
	
private:

};