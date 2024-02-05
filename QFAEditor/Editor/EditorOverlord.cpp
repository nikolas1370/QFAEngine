#include "EditorOverlord.h"

#include <Render/Time.h>
#include <Overlord/Overlord.h>

QFAShaderCompiler QFAEditorOverlord::compiler;

void QFAEditorOverlord::Init()
{
    /*
        this need to get time compiling shaders 
    */
    QTime::Init();

    compiler.ProcessShaders();

    QFAOverlord::Init(compiler.ShaderData);
}

void QFAEditorOverlord::StartLife()
{

    QFAOverlord::StartLife();






    compiler.EndLife();
}
