#pragma once
#include <Object/Class.h>
class QFALevel;
class QObject;
#include <Object/Object.h>
class QFAEXPORT QFAEngineGameCode
{
	friend QFALevel;
	friend QObject;
	friend QObject* NewObject(int classId);
	static QFAGameCodeFunctions* GameCodeAPIFunction;
protected:
	static void SetAPI(QFAGameCodeFunctions* api);
	static QFAGameCodeFunctions* GetAPI();
};