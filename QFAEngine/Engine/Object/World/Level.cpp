#include "pch.h"
#include "Level.h"
#include <Object/Actor/Actor.h>
#include <Overlord/ContentManager.h>
#include <Tools/String.h>
#include <Object/Actor/StaticMeshActor.h>
#include <Overlord/ContentManager.h>
#include <Overlord/GameCode.h>

std::vector<QFALevel::StringOffset> QFALevel::SuperString::StringInfos;

QFALevel::SuperString QFALevel::Ss;
std::array<QFALevel::SActor, QFALevel::MaxActorList> QFALevel::ActorList;
std::vector<QFALevel::Id> QFALevel::ActorClassList;
std::vector<QFALevel::Id> QFALevel::MeshPathList;
std::vector<unsigned int> QFALevel::PathClassList;

void QFALevel::WriteStartFile(size_t AmountActor, QFAFile* file)
{
	QFAContentManager::QFAContentFile cf;
	QFAContentManager::QFAFileOnDisk fd(QFAContentManager::QFAFileTypes::EFTLevel, QFAContentManager::LevelFileVersion);
	QFAFileSystem::WriteFile(LevelPath, &fd, sizeof(fd));

	QFAFileSystem::OpenFile(LevelPath, file, false);
	QFAFileSystem::AppendFile(file, &AmountActor, sizeof(AmountActor));
}

size_t QFALevel::FindActorClass(size_t classId)
{
	for (size_t i = 0; i < ActorClassList.size(); i++)
		if (ActorClassList[i].Id == classId)
			return ActorClassList[i].Index;

	return 0;
}

size_t QFALevel::FindMeshPath(size_t meshDataFileId)
{
	for (size_t i = 0; i < MeshPathList.size(); i++)
		if (MeshPathList[i].Id == meshDataFileId)
			return MeshPathList[i].Index;

	return 0;
}

QFALevel::QFALevel(std::u32string path)
{
	LevelPath = path;
}

#if QFA_EDITOR_ONLY 
	void QFALevel::SaveLevel(QWorld* world)
	{
		if (ActorClassList.capacity() < 1000)
		{
			ActorClassList.reserve(1000);
			MeshPathList.reserve(1000);
		}
	
		if (!LevelPath.size() || !world->IsValid())
			return;
	
		size_t stringStart = sizeof(QFAContentManager::QFAFileOnDisk) + sizeof(size_t) + (world->Actors.Length() * sizeof(QFALevel::SActor));
		size_t actorProces = 0;
		QFAFile file;
		WriteStartFile(world->Actors.Length(), &file);

		std::u32string zeroString;
		Ss.Push(zeroString, stringStart); // write zero string
		
		for (size_t i = 0; i < world->Actors.Length(); i++)
		{		
			ActorList[actorProces].Position = world->Actors[i]->Position;
			ActorList[actorProces].Rotation = world->Actors[i]->Rotation;
			ActorList[actorProces].Scale = world->Actors[i]->Scale;

			if(world->Actors[i]->NameRedacted)
			{			
				ActorList[actorProces].ActorNameIndex = Ss.GetStringCount();
				Ss.Push(world->Actors[i]->Name, stringStart);
			}

			size_t index;
			if (world->Actors[i]->GetClass()->GetId() == QFAClass::ObjectClasses::StaticMeshActor) 
			{// if actor StaticMeshActor
				index = FindMeshPath(((AStaticMeshActor*)world->Actors[i])->Mesh.GetMeshDataFileId());
				if (index)
					ActorList[actorProces].MeshPathIndex = index;
				else
				{
					MeshPathList.push_back(QFALevel::Id{ (unsigned int)((AStaticMeshActor*)world->Actors[i])->Mesh.GetMeshDataFileId(),
														 (unsigned int)Ss.GetStringCount() });
					ActorList[actorProces].MeshPathIndex = MeshPathList.back().Index;
					QFAContentManager::QFAContentFile& cf = QFAContentManager::GetFile(((AStaticMeshActor*)world->Actors[i])->Mesh.GetMeshDataFileId());
					if (cf.Id < 0)
						stopExecute("");
				
					Ss.Push(cf.path, stringStart);
				}
			}

			index = FindActorClass(world->Actors[i]->GetClass()->GetId());
			if (index)
				ActorList[actorProces].ActorClassNameIndex = index;
			else
			{
				ActorClassList.push_back(QFALevel::Id{ (unsigned int)world->Actors[i]->GetClass()->GetId(), Ss.GetStringCount() });
				ActorList[actorProces].ActorClassNameIndex = ActorClassList.back().Index;
				std::u32string className = QFAString::CharsTo32Chars(world->Actors[i]->GetClass()->GetName());
				Ss.Push(className, stringStart);				
			}

			if (actorProces >= MaxActorList) // 
			{
				actorProces = 0;
				QFAFileSystem::AppendFile(&file, &ActorList, MaxActorList * sizeof(QFALevel::SActor));
			}

			actorProces++;
		}
	
		if (actorProces) // write all not written Actor
			QFAFileSystem::AppendFile(&file, &ActorList, actorProces * sizeof(QFALevel::SActor));
	
		for (int i = 0; i < (int)Ss.GetStringBlocksCount() - 1; i++)
			QFAFileSystem::AppendFile(&file, Ss.GetStringBlocks(i), Ss.GetMaxCountSymbolPerBlock() * sizeof(char32_t));
	
		QFAFileSystem::AppendFile(&file, Ss.GetLastStringBlocks(), Ss.GetCountSymbolInLastBlock() * sizeof(char32_t));
	
		size_t StringInfosSize = (size_t)Ss.GetStringCount();
		QFAFileSystem::AppendFile(&file, Ss.GetStringInfoData(), StringInfosSize * sizeof(StringOffset));	
		QFAFileSystem::AppendFile(&file, &StringInfosSize, sizeof(size_t));

		QFAContentManager::QFAContentFolder& cf = QFAContentManager::GetFolder(std::filesystem::path(LevelPath).parent_path().u32string());
		QFAContentManager::AddFile(cf.Id, LevelPath, QFAContentManager::QFAFileTypes::EFTLevel, this);

		Ss.DeleteAll();
		ActorClassList.clear();
		MeshPathList.clear();
	}
	
#endif

QWorld* QFALevel::GetWorld(bool silence)
{
	if (!LevelPath.size())
		return nullptr;

	QFAFile File;
	QFAFileSystem::LoadFile(LevelPath, &File);
	char* fileStart = (char*)File.GetData();
	size_t amountActor = *(size_t*)(fileStart + sizeof(QFAContentManager::QFAFileOnDisk));
	size_t amountString = *(size_t*)(fileStart + File.GetFileSize() - sizeof(size_t));
	SActor* actorList = (SActor*)(fileStart + sizeof(QFAContentManager::QFAFileOnDisk) + sizeof(size_t));
	StringOffset* si = (StringOffset*)(fileStart + File.GetFileSize() - (sizeof(StringOffset) * amountString) - sizeof(size_t));

	PathClassList.resize(amountString);

	QWorld* world = NewObject<QWorld>();
	if(silence)
		world->SilenceWorld = true;

	world->Actors.Reserve(amountActor);
	QFAGameCodeFunctions* gApi = QFAEngineGameCode::GetAPI();
	if (!gApi)
		stopExecute("");

	for (size_t i = 0; i < amountActor; i++)
	{
		QActor* actor = nullptr;
		if (actorList[i].MeshPathIndex) // actor is AStaticMeshActor
		{
			actor = (QActor*)gApi->CreateObject(QFAClass::ObjectClasses::StaticMeshActor);
			if (!actor)
				stopExecute("")

				AStaticMeshActor* sAMesh = (AStaticMeshActor*)actor;

			QFAMeshData* meshData = nullptr;
			if (PathClassList[actorList[i].MeshPathIndex]) // if hawe fileId
			{
				QFAContentManager::QFAContentFile& cf = QFAContentManager::GetFile(PathClassList[actorList[i].MeshPathIndex]);
				meshData = (QFAMeshData*)cf.file;
			}
			else
			{
				char32_t* meshPath = (char32_t*)(fileStart + si[actorList[i].MeshPathIndex]);
				std::u32string lox(meshPath);
				meshData = QFAContentManager::GetMesh(lox);
				PathClassList[actorList[i].MeshPathIndex] = meshData->FIleid;
			}

			sAMesh->SetMesh(meshData);
		}
		else
		{
			if (PathClassList[actorList[i].ActorClassNameIndex]) // if have classId
			{
				actor = (QActor*)QFAEngineGameCode::GetAPI()->CreateObject(PathClassList[actorList[i].ActorClassNameIndex]);
				if (!actor)
					stopExecute("");
			}
			else
			{
				char32_t* className = (char32_t*)(fileStart + si[actorList[i].ActorClassNameIndex]);
				std::u32string classNameU32(className);
				actor = (QActor*)QFAEngineGameCode::GetAPI()->CreateObjectByName(QFAString::U32stringToString(classNameU32).c_str());
				if (!actor)
					stopExecute("");

				PathClassList[actorList[i].ActorClassNameIndex] = actor->GetClass()->GetId();
			}
		}

		if (actorList[i].ActorNameIndex)// if actor name be redacted in editor
		{
			char32_t* className = (char32_t*)(fileStart + si[actorList[i].ActorNameIndex]);
			actor->Name = std::u32string(className);
		}
		else // not redacted, be automatically generated
			actor->Name = QFAString::CharsTo32Chars(actor->GetClass()->GetName());

		if (actor)
		{
			actor->SetActorPosition(actorList[i].Position);
			actor->SetActorRotation(actorList[i].Rotation);
			actor->SetActorScale(actorList[i].Scale);
			world->AddActor(actor);
		}
		else
			std::cout << "QFALevel::GetWorld !actor\n";
	}

	PathClassList.clear();
	return world;
}

QWorld* QFALevel::GetWorld()
{	
	return GetWorld(false);
}

void QFALevel::SuperString::Push(std::u32string &str, unsigned int stringOffset)
{
	if (StringInfos.capacity() < StringInfoStartcapacity)
		StringInfos.reserve(StringInfoStartcapacity);

	if (!StringBlocks.size())
	{
		CountSymbol = 0;
		StringBlocks.push_back(new char32_t[BlockSymbolAmount]);
	}

	StringInfos.push_back((StringOffset)(stringOffset + (TotalCountSymbol * sizeof(char32_t))));

	char32_t* curentBlock = StringBlocks.back();
	size_t strSize = str.size() + 1;
	if (strSize > BlockSymbolAmount)
		stopExecute("QFALevel::SuperString::Push");

	if (CountSymbol + strSize <= BlockSymbolAmount)
	{
		memcpy(&curentBlock[CountSymbol], str.data(), strSize * sizeof(char32_t));
		CountSymbol += strSize;
		TotalCountSymbol += strSize;
	}
	else
	{
		size_t remainedSize = BlockSymbolAmount - CountSymbol;
		memcpy(&curentBlock[CountSymbol], str.data(), remainedSize * sizeof(char32_t));
		TotalCountSymbol += remainedSize;
	
		StringBlocks.push_back(new char32_t[BlockSymbolAmount]);
		curentBlock = StringBlocks.back();

		memcpy(curentBlock, &str.data()[remainedSize], (strSize - remainedSize) * sizeof(char32_t));
		CountSymbol = strSize - remainedSize;
		TotalCountSymbol += CountSymbol;
	}
}
