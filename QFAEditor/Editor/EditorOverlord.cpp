#include "EditorOverlord.h"

#include <Render/Time.h>
#include <Overlord/Overlord.h>
#include <Render/UI/Text.h>
#include <EditorWindows/MainEditorWindow.h>
#include <EditorFileStorage.h>

QFAShaderCompiler QFAEditorOverlord::compiler;
std::thread* QFAEditorOverlord::LoadThread;

bool QFAEditorOverlord::AllFilesLoad = false;
bool QFAEditorOverlord::NewLoadText = false;
std::u32string QFAEditorOverlord::LoadText;
std::u32string QFAEditorOverlord::LoadText_2;
QFAEditorMainWindow* QFAEditorOverlord::MainWindow;


void QFAEditorOverlord::StartLife()
{
    Init();
    LoadThread = new std::thread(QFAEditorOverlord::PrepareToWork);
    QFAOverlord::StartLife();

    delete MainWindow;
    compiler.EndLife();
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
    NewLoadText = true;
    LoadText = U"Load file : ";
    QFAEditorFileStorage::LoadEditorFiles(LoadText_2, NewLoadText);
    AllFilesLoad = true;
}

void QFAEditorOverlord::StartFrame()
{
    if (AllFilesLoad)
    {
        AllFilesLoad = false;
        MainWindow->CreateUI();
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


