#include "pch.h"
#include "Window.h"
#include <Core/Window/Viewport.h>

#if QFA_EDITOR_ONLY
QFAWindow* QFAWindow::GameWindow = nullptr;
#endif

QFAWindow::QFAWindow()
{
	QFAViewport* vp = new QFAViewport;
	AddViewport(vp);
	RegularWindow = false;

}

QFAWindow* QFAWindow::GetWindow()
{
#if QFA_EDITOR_ONLY
	return GameWindow;
#else

#endif
	
}
