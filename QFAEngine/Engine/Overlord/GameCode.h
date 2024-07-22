#pragma once
#include <Object/Class.h>
class QFALevel;
class QFAEXPORT QFAEngineGameCode
{
	friend QFALevel;
	static QFAGameCodeFunctions* GameCodeAPIFunction;
protected:
	static void SetAPI(QFAGameCodeFunctions* api);
	static QFAGameCodeFunctions* GetAPI();
};