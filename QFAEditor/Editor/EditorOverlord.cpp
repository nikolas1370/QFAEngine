#include "epch.h"
#include "EditorOverlord.h"

#include <Render/Time.h>
#include <Overlord/Overlord.h>
#include <Render/UI/Text.h>
#include <EditorWindows/MainEditorWindow.h>
#include <EditorFileStorage.h>
#include <GameCodeTool/GameCodeCompiler.h>

QFAShaderCompiler QFAEditorOverlord::compiler;
std::thread* QFAEditorOverlord::LoadThread;

bool QFAEditorOverlord::InitializationDon = false;
bool QFAEditorOverlord::NewLoadText = false;
std::u32string QFAEditorOverlord::LoadText;
std::u32string QFAEditorOverlord::LoadText_2;
QFAEditorMainWindow* QFAEditorOverlord::MainWindow;
bool QFAEditorOverlord::IsInit = false;
QFAEditorFileStorage QFAEditorOverlord::Storage;

void QFAEditorOverlord::StartLife()
{
    if (IsInit)
        return;
    else
        IsInit = true;

    Init();
    LoadThread = new std::thread(QFAEditorOverlord::PrepareToWork);
    QFAOverlord::StartLife();

    delete MainWindow;
    compiler.EndLife();
    QFAEditorFileStorage::EndLife();
}



void QFAEditorOverlord::Init()
{
    /*
        this need to get time compiling shaders 
    */
    QTime::Init();
    compiler.ProcessShaders();
    QFAOverlord::Init(compiler.ShaderData, false, QFAEditorOverlord::StartFrame, QFAEditorOverlord::EndFrame, &Storage);

    MainWindow = new QFAEditorMainWindow;    
}

void QFAEditorOverlord::PrepareToWork()
{
    NewLoadText = true;
    LoadText = U"Load Game code";
    QFAGameCode::LoadCode();
    LoadText = U"Load file : ";
    QFAEditorFileStorage::LoadEditorFiles(LoadText_2, NewLoadText);
    InitializationDon = true;
}

void QFAEditorOverlord::StartFrame()
{
    if (InitializationDon)
    {
        InitializationDon = false;
        MainWindow->CreateMainEdirorUI();
    }
    else if(NewLoadText)
    {
        
        NewLoadText = false;
        MainWindow->ChangeLoadInfo(LoadText, LoadText_2);        
    }
}

void QFAEditorOverlord::EndFrame()
{


}


