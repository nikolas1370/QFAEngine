#include "pch.h"
#include "GameCode.h"

QFAGameCodeFunctions* QFAEngineGameCode::GameCodeAPIFunction = nullptr;

void QFAEngineGameCode::SetAPI(QFAGameCodeFunctions* api)
{
	GameCodeAPIFunction = api;
}

QFAGameCodeFunctions* QFAEngineGameCode::GetAPI()
{
	return GameCodeAPIFunction;
}
