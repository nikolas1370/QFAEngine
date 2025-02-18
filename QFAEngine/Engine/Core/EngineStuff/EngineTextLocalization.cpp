#include "pch.h"
#include "EngineTextLocalization.h"

QFAEngineLocalization* QFAEngineLocalization::GameModuleLocal = nullptr;

QFAEngineLocalization::QFAEngineLocalization()
{
}

QFAEngineLocalization::~QFAEngineLocalization()
{
}

void QFAEngineLocalization::SetLocalizationInstance(QFAEngineLocalization* localInstance)
{
	GameModuleLocal = localInstance;
}