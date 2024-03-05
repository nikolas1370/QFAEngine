#pragma once

#include "ShaderCompiler/ShaderCompiler.h"
class QFAEditorMainWindow;
class QFAEditorOverlord
{
	static QFAShaderCompiler compiler;
	QFAEditorMainWindow* mainWindow;
public:
	

	static void StartLife();
	static void Init();
private:
	
};

