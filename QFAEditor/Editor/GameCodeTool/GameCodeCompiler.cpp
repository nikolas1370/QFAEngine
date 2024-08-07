#include "epch.h"
#include "GameCodeCompiler.h"

#include <Tools/File/FileSystem.h>
#include <Windows.h> 
#include <iostream>
#include <processthreadsapi.h>
#include <EngineStuff/VulkanSuff.h>
#include <Object/Actor/Actor.h>
#include <Object/Class.h>


QFAGameCode::DllFile QFAGameCode::CurentDllFile = QFAGameCode::DllFile::None;
bool QFAGameCode::DllWasCompiled = false;


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

const wchar_t* QFAGameCode::HotFolder_1Path = L"Source/x64/debug/HotReload_1";
const wchar_t* QFAGameCode::HotFolder_2Path = L"Source/x64/debug/HotReload_2";

const wchar_t* QFAGameCode::DllHotPath = L"Source/x64/debug/GameCode.dll";
const wchar_t* QFAGameCode::DllHot_1Path = L"Source/x64/debug/HotReload_1/GameCode.dll";
const wchar_t* QFAGameCode::DllHot_2Path = L"Source/x64/debug/HotReload_2/GameCode.dll";
const wchar_t* QFAGameCode::PdbHot_1Path = L"Source/x64/debug/HotReload_1/GameCode.pdb";
const wchar_t* QFAGameCode::PdbHot_2Path = L"Source/x64/debug/HotReload_2/GameCode.pdb";
#else
const char* QFAGameCode::CopyDllFrom = "Source/x64/release/GameCode.dll";
const char* QFAGameCode::CopyPdbFrom = "Source/x64/release/GameCode.pdb";
const char* QFAGameCode::CopyDllIn = "../x64/Release/GameCode.dll";
const char* QFAGameCode::CopyPdbIn = "../x64/Release/GameCode.pdb";

const wchar_t* QFAGameCode::HotFolder_1Path = L"Source/x64/release/HotReload_1";
const wchar_t* QFAGameCode::HotFolder_2Path = L"Source/x64/release/HotReload_2";

const wchar_t* QFAGameCode::DllHotPath = L"Source/x64/release/GameCode.dll";
const wchar_t* QFAGameCode::DllHot_1Path = L"Source/x64/release/HotReload_1/GameCode.dll";
const wchar_t* QFAGameCode::DllHot_2Path = L"Source/x64/release/HotReload_2/GameCode.dll";
const wchar_t* QFAGameCode::PdbHot_1Path = L"Source/x64/release/HotReload_1/GameCode.pdb";
const wchar_t* QFAGameCode::PdbHot_2Path = L"Source/x64/release/HotReload_2/GameCode.pdb";
#endif

void* QFAGameCode::GameCodeModule = nullptr; // HMODULE
void* QFAGameCode::OldGameCodeModule = nullptr;

bool QFAGameCode::CompileInWork = false;

bool QFAGameCode::Compile()
{
    if (CompileInWork)
        return false;

    CompileInWork = true;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));// /t:rebuild    
    TCHAR lox2[] = L"\"C:/Program Files/Microsoft Visual Studio/2022/Community/MSBuild/Current/Bin/MSBuild.exe\" Source/GameCode.vcxproj /p:configuration=debug /p:platform=x64";
    std::filesystem::remove(CopyPdbFrom); // without it got compile error
    std::error_code xa;

    if (!CreateProcessW(0,   // No module name (use command line)
        lox2,
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi)           // Pointer to PROCESS_INFORMATION structure
        )
    {
        printf("CreateProcess failed (%d).\n", GetLastError());
        return false;
    }

    // Wait until child process exits.
    WaitForSingleObject(pi.hProcess, INFINITE);
    bool succeed = false;

    DWORD ExitCode;
    GetExitCodeProcess(pi.hProcess, &ExitCode);
    if (!ExitCode)
        succeed = true;

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    if (succeed)
        DllWasCompiled = true;

    CompileInWork = false;
    return succeed;
}

void QFAGameCode::ReplacementOldDllObject()
{
    if (OldGameCodeModule)
    {
        FARPROC Functions = (FARPROC)GetProcAddress((HMODULE)OldGameCodeModule, "___QFAGAMECODEEXPORTFUNCTIONGETFUNCTIONS___");
        if (Functions)
        {
            QFAGameCodeFunctions* oldApi = (QFAGameCodeFunctions*)Functions();

            std::vector<QFAClass*>& oldClasses = oldApi->GetClassList(); 
            for (size_t i = 0; i < oldClasses.size(); i++)
            {
                if (QFAClass* newClasses = FindInNewClass(oldClasses[i]->GetName())) 
                {// write check if old and current class have one parent tree if not du some
                    if (oldClasses[i]->GetBaseOn() == QFAClass::Actor && newClasses->GetBaseOn() == QFAClass::Actor)
                    {
                        std::vector<QObject*>& OldActorList = oldApi->GetGameObjectList(oldClasses[i]->GetId());
                        for (size_t j = 0; j < OldActorList.size(); j++)
                            OldActorList[j]->ReplaceMe(GetAPI()->CreateObject(newClasses->GetId()));
                    }
                    else if(oldClasses[i]->GetBaseOn() == QFAClass::Actor && newClasses->GetBaseOn() != QFAClass::Actor)
                        stopExecute("Restart editor")
                }
                else 
                {
                    if (oldClasses[i]->GetBaseOn() == QFAClass::Actor)
                    {
                        std::vector<QObject*>& OldActorList = oldApi->GetGameObjectList(oldClasses[i]->GetId());
                        for (size_t j = 0; j < OldActorList.size(); j++)
                            OldActorList[j]->ReplaceMe(NewObject<QActor>());
                    }                    
                }
            }

            FARPROC freeFun = (FARPROC)GetProcAddress((HMODULE)OldGameCodeModule, "___QFAGAMECODEEXPORTFUNCTIONFreeClasses___");
            if (freeFun)
                freeFun();
            else
                stopExecute("");

            FreeLibrary((HMODULE)OldGameCodeModule);
            OldGameCodeModule = nullptr;
        }
        else
        {
            stopExecute("")
                return;
        }
    }
}

bool QFAGameCode::CopyDll(DllFile f)
{
    std::filesystem::copy_options co = std::filesystem::copy_options::update_existing;
    if (f == DllFile::None)
        return false;
    else if (f == DllFile::GameCode)
    {
        try
        {
            std::filesystem::copy_file(CopyDllFrom, CopyDllIn, co);
            std::filesystem::copy_file(CopyPdbFrom, CopyPdbIn, co);
        }
        catch (const std::exception& as)
        {
            std::cout << as.what();
            stopExecute("")
                return false;
        }
    }
    else
    {
        std::filesystem::create_directories(f == DllFile::GameCode_HotReload_1 ? HotFolder_1Path : HotFolder_2Path);
        try
        {
            std::filesystem::remove(f == DllFile::GameCode_HotReload_1 ? PdbHot_1Path : PdbHot_2Path); // without it VS lock pdb file
            std::filesystem::copy_file(CopyDllFrom, f == DllFile::GameCode_HotReload_1 ? DllHot_1Path : DllHot_2Path, co);
            std::filesystem::copy_file(CopyPdbFrom, f == DllFile::GameCode_HotReload_1 ? PdbHot_1Path : PdbHot_2Path, co);
        }
        catch (const std::exception& as)
        {
            std::cout << as.what();
            stopExecute("")
                return false;
        }
    }

    return true;
}

QFAClass* QFAGameCode::FindInNewClass(const char* className)
{
    std::vector<QFAClass*>& newClasses = GetAPI()->GetClassList();
    for (size_t i = 0; i < newClasses.size(); i++)
        if (std::strcmp(className, newClasses[i]->GetName()) == 0)
            return newClasses[i];

    return nullptr;
}



void (*QFAGameCode::CompileCallback)(QFAGameCode::CompileStatus);
void QFAGameCode::CompileGameCode(void (*callback)(CompileStatus))
{
    if (CompileInWork)
        return callback(CompileStatus::CompilationStillWork);

    CompileCallback = callback;
    if (Compile())
    {
        if (LoadCode())
            ReplacementOldDllObject();

        CompileCallback(CompileStatus::OK);
    }
    else
        CompileCallback(CompileStatus::Error);
}

bool QFAGameCode::LoadCode()
{
    bool needReplacement = false;
    LPCWSTR loadPath = nullptr;
    if (CurentDllFile == DllFile::None)
    {
        bool main = std::filesystem::exists(CopyDllIn);
        bool hoot = std::filesystem::exists(CopyDllFrom);

        if (main && hoot)
        {
            time_t timeMain = std::filesystem::last_write_time(CopyDllIn).time_since_epoch().count();
            time_t timeHot = std::filesystem::last_write_time(CopyDllFrom).time_since_epoch().count();
            if (timeMain < timeHot)
                if (!CopyDll(DllFile::GameCode))
                    return false;
        }
        else if (hoot)
        {
            if (!CopyDll(DllFile::GameCode))
                return false;
        }
        else if(!main)
            return false;

        CurentDllFile = DllFile::GameCode;
        loadPath = LoadDllPath;
    }
    else if (CurentDllFile == DllFile::GameCode)
    {
        if (!DllWasCompiled)
            return false;

        if (std::filesystem::exists(CopyDllFrom))
        {
            if (!CopyDll(DllFile::GameCode_HotReload_1))
                return false;

            CurentDllFile = DllFile::GameCode_HotReload_1;
            loadPath = DllHot_1Path;
        }
        else
            return false;
    }
    else
    {
        if (!DllWasCompiled)
            return false;

        if (CurentDllFile == DllFile::GameCode_HotReload_1)
        {
            if (!CopyDll(DllFile::GameCode_HotReload_2))
                return false;

            CurentDllFile = DllFile::GameCode_HotReload_2;
            loadPath = DllHot_2Path;
        }
        else
        {
            if (!CopyDll(DllFile::GameCode_HotReload_1))
                return false;

            CurentDllFile = DllFile::GameCode_HotReload_1;
            loadPath = DllHot_1Path;
        }
    }

    DllWasCompiled = false;
    HMODULE mod = LoadLibraryW(loadPath);
    if (mod)
    {
        auto Functions = (QFAGameCodeFunctions* (*)(QFAClass**))GetProcAddress(mod, "___QFAGAMECODEEXPORTFUNCTIONGETFUNCTIONS___");
        if (Functions)
            SetAPI(Functions(QFAClass::GetEngineClassList()));
        else
        {
            FreeLibrary(mod);
            stopExecute("dead end\n");
            return false;
        }

        if (GameCodeModule)
        {
            needReplacement = true;
            OldGameCodeModule = GameCodeModule;
        }

        GameCodeModule = (void*)mod;
    }
    else
    {
        stopExecute("LoadLibraryW failed");
        return false;
    }

    return needReplacement;
}
