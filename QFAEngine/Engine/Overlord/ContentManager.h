#pragma once
#include <string>
#include <Tools/Stuff.h>
class QFAMeshData;
class QFAOverlord;
class QFAEXPORT QFAContentManager
{
	friend QFAOverlord;
	static QFAContentManager* Manager; // set in Overlord::Init
protected:
	virtual QFAMeshData* GetMeshPath(std::u32string path) = 0;
public:
	// if return nullptr mesh not found
	static QFAMeshData* GetMesh(std::u32string path);
};