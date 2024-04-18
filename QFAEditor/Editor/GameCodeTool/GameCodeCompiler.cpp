#include "GameCodeCompiler.h"

#include <Tools/File/FileSystem.h>

#include <Windows.h> 
#include <iostream>

/*

    HMODULE mod = LoadLibraryW(L"lopoid.dll");
    if (mod)
    {
        FARPROC Add = (FARPROC)GetProcAddress(mod, "lox");
        if(Add)
        {

            std::cout << Add() << "\n";
        }

        FreeLibrary(mod);
    }



    
    /*
    std::cout << typeid(QFAEditorMainWindow).name() << "\n";
    std::cout << typeid(QFAEditorMainWindow).raw_name() << "\n";
    * /



*/


#include <Object/Object.h>
void (*GameCodeCompiler::CompileCallback)(GameCodeCompiler::CompileStatus);
void GameCodeCompiler::CompileGameCode(void (*callback)(CompileStatus))
{// SourceFolder
	CompileCallback = callback;
    HMODULE mod = LoadLibraryW(L"GameCode.dll");
    if (mod)
    {
        FARPROC Functions = (FARPROC)GetProcAddress(mod, "QFAGetFunctions");
        if (Functions)
        {
            QFAGameCodeFunctions* funs = (QFAGameCodeFunctions*)Functions();
            
            std::vector<QFAClassInfoBase*>* gameClassList = funs->GetGameClassList();            
            QFAClassInfoBase** classInfo = gameClassList->data();
            
            for (size_t i = 0; i < gameClassList->size(); i++)
            {
                QObject* obj = funs->CreateObject(classInfo[i]->GetClassId());
                funs->DeleteObject(obj);
            }
        }
        else
        {
            std::cout <<"dead end\n";
        }

        FreeLibrary(mod);
    } 
}
/*
    only  class NameClass {}
    or	  class NameClass : ParentNameClass {}
*/
void GameCodeCompiler::AnalyzeFile(std::filesystem::path filePath)
{
    QFAFile file;
    
    if (QFAFileSystem::LoadFile(filePath.u32string(), &file))
        return;

    char* text = (char*)file.GetData();
    size_t textSize = file.GetFileSize();


}