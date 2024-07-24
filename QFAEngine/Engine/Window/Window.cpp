#include "pch.h"
#include "Window.h"
#include <Window/Viewport.h>

#if QFA_EDITOR_ONLY
QFAWindow* QFAWindow::GameWindow;
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