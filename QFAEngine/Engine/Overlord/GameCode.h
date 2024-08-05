#pragma once
#include <Object/Class.h>
class QFALevel;
class QObject;
class QFAEXPORT QFAEngineGameCode
{
	friend QFALevel;
	friend QObject;
	static QFAGameCodeFunctions* GameCodeAPIFunction;
protected:
	static void SetAPI(QFAGameCodeFunctions* api);
	static QFAGameCodeFunctions* GetAPI();
};