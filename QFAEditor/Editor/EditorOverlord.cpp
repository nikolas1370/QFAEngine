#include "epch.h"
#include "EditorOverlord.h"

#include <Overlord/Time.h>
#include <Overlord/Overlord.h>
#include <UI/Text.h>
#include <EditorWindows/MainEditorWindow.h>
#include <EditorFileStorage.h>
#include <GameCodeTool/GameCodeCompiler.h>

QFAShaderCompiler QFAEditorOverlord::compiler;


bool QFAEditorOverlord::InitializationDon = false;
std::u32string QFAEditorOverlord::LoadText;
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
    std::jthread LoadThread(QFAEditorOverlord::PrepareToWork);
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
    QFAOverlord::Init(compiler.ShaderData, false, QFAEditorOverlord::StartFrame, QFAEditorOverlord::EndFrame);

    MainWindow = new QFAEditorMainWindow;    
}

void QFAEditorOverlord::PrepareToWork()
{

    /*
    
    std::lock_guard
    
    отут
    
    
    */

    LoadText = U"Load Game code";
    QFAGameCode::LoadCode();
    LoadText = U"Load file : ";
    QFAEditorFileStorage::LoadContent(U"Content", [](std::u32string string)
    {
        MainWindow->ChangeLoadInfo(LoadText, string);
    });
    InitializationDon = true;
}

void QFAEditorOverlord::StartFrame()
{
    if (InitializationDon)
    {
        InitializationDon = false;
        MainWindow->CreateMainEdirorUI();
    }
}

void QFAEditorOverlord::EndFrame()
{


}


