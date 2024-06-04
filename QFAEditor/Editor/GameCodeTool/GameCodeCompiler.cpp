#include "GameCodeCompiler.h"

#include <Tools/File/FileSystem.h>
#include <Windows.h> 
#include <iostream>
#include <processthreadsapi.h>

#if _DEBUG
const char* QFAGameCode::CopyDllFrom = "Source/x64/debug/GameCode.dll";
const char* QFAGameCode::CopyDllIn = "../x64/Debug/GameCode.dll";
#else
const char* QFAGameCode::CopyDllFrom = "Source/x64/release/GameCode.dll";
const char* QFAGameCode::CopyDllIn = "../x64/Release/GameCode.dll";
#endif

void* QFAGameCode::GameCodeModule = NULL; // HMODULE
QFAGameCodeFunctions* QFAGameCode::GameCodeAPIFunction = nullptr;

bool QFAGameCode::Compile()
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    // /t:rebuild 
    TCHAR lox2[] = L"\"C:/Program Files/Microsoft Visual Studio/2022/Community/MSBuild/Current/Bin/MSBuild.exe\" Source/GameCode.vcxproj /p:configuration=debug /p:platform=x64";

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
    if (!ExitCode && std::filesystem::exists(CopyDllFrom))
    {
        std::filesystem::copy_options co = std::filesystem::copy_options::update_existing;
        std::filesystem::copy_file(CopyDllFrom, CopyDllIn, co);
        succeed = true;
    }
    
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return succeed;
}


#include <Object/Object.h>
void (*QFAGameCode::CompileCallback)(QFAGameCode::CompileStatus);
void QFAGameCode::CompileGameCode(void (*callback)(CompileStatus))
{
	CompileCallback = callback;
    if (!Compile())
        return;
    
    LoadCode();
}

void QFAGameCode::LoadCode()
{

    HMODULE mod = LoadLibraryW(L"GameCode.dll");
    if (mod)
    {
        std::cout << "lox in fish\n";
        FARPROC Functions = (FARPROC)GetProcAddress(mod, "QFAGetFunctions");
        if (Functions)
            GameCodeAPIFunction = (QFAGameCodeFunctions*)Functions();
        else
        {
            FreeLibrary(mod);
            std::cout << "dead end\n";
            return;
        }

        if (GameCodeModule)
        {
            /*

                do some stuff

            */
            FreeLibrary((HMODULE)GameCodeModule);
        }
      
        GameCodeModule = (void*)mod;
    }
}
