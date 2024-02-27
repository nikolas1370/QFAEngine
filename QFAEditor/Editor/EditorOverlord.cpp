#include "EditorOverlord.h"

#include <Render/Time.h>
#include <Overlord/Overlord.h>
#include <Render/UI/Text.h>

QFAShaderCompiler QFAEditorOverlord::compiler;

void QFAEditorOverlord::Init()
{
    /*
        this need to get time compiling shaders 
    */
    QTime::Init();

    compiler.ProcessShaders();
    QFAOverlord::Init(compiler.ShaderData);

    QFAText::SFont* font;
    if (QFAText::ELoadFontResult res = QFAText::LoadFont("SomeFont/Roboto-Regular.ttf", font))
        stopExecute(res);
}

void QFAEditorOverlord::StartLife()
{

    QFAOverlord::StartLife();






    compiler.EndLife();
}
