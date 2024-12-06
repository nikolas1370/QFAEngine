#pragma once
#include <Object/Class.h>
#include <UI/TextLocalization.h>
/*
    editor call it before work with game module
    this file need include in one game module cpp file
*/
extern "C" __declspec(dllexport) void* ___QFAEngineGetClassInstance___()
{// game module return own ClassInstance
    return QFAClassInstance::GetClassInstance();
}

extern "C" __declspec(dllexport) void* ___QFAEngineGetLocalizationInstance___()
{
    return QFALocalization::GetLocalizationInstance();
}