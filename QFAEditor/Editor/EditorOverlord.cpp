#include "epch.h"
#include "EditorOverlord.h"

#include <Core/Time.h>
#include <Core/QFA.h>
#include <UI/Text.h>
#include <EditorWindows/MainEditorWindow.h>
#include <EditorFileStorage.h>
#include <GameCodeTool/GameCodeCompiler.h>

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

    QFAWindowParameter wcp;
    wcp.name = "QFAEditor";
    wcp.height = 200;
    wcp.width = 500;
    wcp.inCenter = true;
    wcp.decorated = false;

    QFAOverlord::Init(wcp, U"EngineFiles/Shaders/", "EngineFiles/Fonts/Roboto-Regular.ttf");
    
    MainWindow = new QFAEditorMainWindow;

    MainWindow->SetWindow(QFAEditorWindow::GetMainWindows());

    std::jthread LoadThread(QFAEditorOverlord::PrepareToWork);

    QFAOverlord::StartLife(QFAEditorOverlord::StartFrame, QFAEditorOverlord::EndFrame);

    delete MainWindow;
    QFAEditorFileStorage::EndLife();
}

void QFAEditorOverlord::PrepareToWork()
{
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
