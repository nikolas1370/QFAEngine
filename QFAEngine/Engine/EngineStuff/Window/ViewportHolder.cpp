#include "pch.h"
#if QFA_EDITOR_ONLY
#include "ViewportHolder.h"

void QFAViewportHolder::Settup(int windowWidth, int windowHeight)
{
	SettupInside(windowWidth, windowHeight);
	HoldedWindow.SetSize(Width, Height);
	for (size_t i = 0; i < HoldedWindow.Viewports.Length(); i++)
		HoldedWindow.Viewports[i]->SettupInside(Width, Height);
}

QFAViewportHolder::QFAViewportHolder()
{
	RegularViewport = false;
	HoldedWindow.Holder = this;
	QFAWindow::GameWindow = &HoldedWindow;
	QFAWindow::GetWindow()->GetViewport(0);
}

QFAViewportHolder::~QFAViewportHolder()
{
}

#endif