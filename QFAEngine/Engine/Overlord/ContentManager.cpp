#include "pch.h"
#include "ContentManager.h"

QFAContentManager* QFAContentManager::Manager;
QFAMeshData* QFAContentManager::GetMesh(std::u32string path)
{
    return Manager->GetMeshPath(path);
}
