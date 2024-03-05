#include "EditorOverlord.h"

#include <Render/Time.h>
#include <Overlord/Overlord.h>
#include <Render/UI/Text.h>
#include <EditorWindows/MainEditorWindow.h>

QFAShaderCompiler QFAEditorOverlord::compiler;

void QFAEditorOverlord::Init()
{
    /*
        this need to get time compiling shaders 
    */
    QTime::Init();

    compiler.ProcessShaders();
    QFAOverlord::Init(compiler.ShaderData, false);

    QFAEditorMainWindow* mainWindow = new QFAEditorMainWindow;
}

void QFAEditorOverlord::StartLife()
{
    Init();
    QFAOverlord::StartLife();
    compiler.EndLife();
}
