#pragma once
#include "World.h"
#include <Math/Vector.h>
#include <string>
#include <Tools/Stuff.h>
#include <array>
#include <Tools/File/FileSystem.h>

class SuperString;
class QFAContentManager;
class QFAEXPORT QFALevel
{
	friend SuperString;
	friend QFAContentManager;
	/*
		QFAContentManager::QFAFileOnDisk,
		size_t AmountActor,
		[SActor],
		[char32_t],// c strings store class name or Actor.Name [0, "class SomeClassname", "class SomeClassname2", "class SomeClassname3"] first string element 0 because index 0 rezerved
		[StringOffset]
		size_t count string
	*/

	struct SActor
	{   // index 0 reserved
		unsigned int ActorClassNameIndex = 0; 
		unsigned int ActorNameIndex = 0;
		unsigned int MeshPathIndex = 0; // if actor type is AStaticMeshActor mesh be load from path and set in actor 
		FVector Position;
		FVector Rotation;
		FVector Scale;
	};

	typedef unsigned int StringOffset;

	class SuperString
	{
		static const size_t StringInfoStartcapacity = 10000;
		static const size_t BlockSymbolAmount = 1000;

		size_t CountSymbol = 0; // count symbol in last index StringBlocks
		size_t TotalCountSymbol = 0;
		std::vector<char32_t*> StringBlocks;
		static std::vector<StringOffset> StringInfos; // string Offset from start file

	public:
		// stringOffset offset(in byte)  from start file to first element string
		void Push(std::u32string &str, unsigned int stringOffset);
		void DeleteAll()
		{
			for (size_t i = 0; i < StringBlocks.size(); i++)
				delete[] StringBlocks[i];

			StringBlocks.clear();
			StringInfos.clear();
			CountSymbol = 0;
			TotalCountSymbol = 0;
		}	

		inline unsigned int GetStringCount()
		{
			return (unsigned int)StringInfos.size();
		}

		inline StringOffset* GetStringInfoData()
		{
			return StringInfos.data();
		}

		inline unsigned int GetStringBlocksCount()
		{
			return (unsigned int)StringBlocks.size();
		}

		char32_t* GetStringBlocks(size_t index)
		{
			return StringBlocks[index];
		}

		char32_t* GetLastStringBlocks()
		{
			return StringBlocks.back();
		}

		size_t GetCountSymbolInLastBlock()
		{
			return CountSymbol;
		}

		inline size_t GetMaxCountSymbolPerBlock()
		{
			return BlockSymbolAmount;
		}

	};
	
	struct Id
	{
		unsigned int Id; // meshData or class id
		unsigned int Index; // string index in Ss(SuperString)
	};
	
	
	static const size_t MaxActorList = 1000;
	static SuperString Ss;

	static std::array<SActor, MaxActorList> ActorList;
	static std::vector<Id> ActorClassList;
	static std::vector<Id> MeshPathList;
	static std::vector<unsigned int> PathClassList; // file id or class id // indexes is MeshPathIndex or ActorClassNameIndex

protected:		
	std::u32string LevelPath;

private:
	void WriteStartFile(size_t AmountActor, QFAFile* file);

	// return for ActorClassNameIndex
	size_t FindActorClass(size_t classId);
	// return for MeshPathIndex
	size_t FindMeshPath(size_t meshDataFileId);
	QFALevel(std::u32string path);
protected:
	QFALevel(){}
	
#if QFA_EDITOR_ONLY 
	void SaveLevel(QWorld* world);
#endif

public:
	// create new world from level
	QWorld* GetWorld();


};
