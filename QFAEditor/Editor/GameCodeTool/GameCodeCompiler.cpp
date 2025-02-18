#include "epch.h"
#include "GameCodeCompiler.h"

#include <Tools/File/FileSystem.h>
#include <Windows.h> 
#include <iostream>
#include <processthreadsapi.h>
#include <Core/EngineStuff/VulkanSuff.h>
#include <Object/Actor/Actor.h>
#include <Object/Class.h>
#include <Core/EngineStuff/EngineTextLocalization.h>

// pdb file need for visual studio EDI https://learn.microsoft.com/en-us/visualstudio/debugger/specify-symbol-dot-pdb-and-source-files-in-the-visual-studio-debugger?view=vs-2022
const wchar_t* QFAGameCode::LoadDllPath = L"GameCode.dll";
/*
    create shipped version
*/
#if _DEBUG
const char* QFAGameCode::CopyDllFrom = "Source/x64/debug/GameCode.dll";
const char* QFAGameCode::CopyPdbFrom = "Source/x64/debug/GameCode.pdb";
const char* QFAGameCode::CopyDllIn = "../x64/Debug/GameCode.dll";
const char* QFAGameCode::CopyPdbIn = "../x64/Debug/GameCode.pdb";

#else
const char* QFAGameCode::CopyDllFrom = "Source/x64/release/GameCode.dll";
const char* QFAGameCode::CopyPdbFrom = "Source/x64/release/GameCode.pdb";
const char* QFAGameCode::CopyDllIn = "../x64/Release/GameCode.dll";
const char* QFAGameCode::CopyPdbIn = "../x64/Release/GameCode.pdb";

#endif

void* QFAGameCode::GameCodeModule;// GameCodeModule is HMODULE, don't wanna include windows.h

bool QFAGameCode::LoadCode()
{
    bool needReplacement = false;
    bool main = std::filesystem::exists(CopyDllIn);
    bool hoot = std::filesystem::exists(CopyDllFrom);

    if (!main && !hoot)
        return false;

    time_t timeMain = std::filesystem::last_write_time(CopyDllIn).time_since_epoch().count();
    time_t timeHot = std::filesystem::last_write_time(CopyDllFrom).time_since_epoch().count();
    if (!main || timeMain < timeHot)
    {
        try
        {
            std::filesystem::copy_file(CopyDllFrom, CopyDllIn, std::filesystem::copy_options::update_existing);
            std::filesystem::copy_file(CopyPdbFrom, CopyPdbIn, std::filesystem::copy_options::update_existing);
        }
        catch (const std::exception& as)
        {
            std::cout << as.what();
            stopExecute("")
                return false;
        }
    }

    HMODULE mod = LoadLibraryW(LoadDllPath);
    if (mod)
    {   // next function located in EditorApiFunction.h and need include in one game module cpp file
        auto GetClassInstance = (QFAEngineClassInstance * (*)())GetProcAddress(mod, "___QFAEngineGetClassInstance___");
        auto LocalFunction = (QFAEngineLocalization * (*)())GetProcAddress(mod, "___QFAEngineGetLocalizationInstance___");
        if (GetClassInstance && LocalFunction)
        {            
            QFAEngineClassInstance::SetGameClassInstance(GetClassInstance());
            GetClassInstance()->InitClasses(QFAEngineClassInstance::GetEngineClass());

            QFAEditorLocalization::SetLocalizationInstance(LocalFunction());
            GameCodeModule = (void*)mod;
        }
        else
        {
            FreeLibrary(mod);
            stopExecute("game module not have ___QFAEngineGetClassInstance___ or ___QFAEngineGetLocalizationInstance___\n");
            return false;
        }        
    }
    else
    {
        stopExecute("LoadLibraryW failed");
        return false;
    }

    return true;
}
