#include <Object/Class.cpp>
#include "main.h"


// editor call it before work with game module 
// need set it in editor but not in build 
extern "C" __declspec(dllexport) void* ___QFAEngineGetLocalizationInstance___()
{
    return QFALocalization::GetLocalizationInstance();
}