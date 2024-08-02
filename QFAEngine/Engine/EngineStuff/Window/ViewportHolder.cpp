#include "pch.h"
#if QFA_EDITOR_ONLY
#include "ViewportHolder.h"

void QFAViewportHolder::Settup(int windowWidth, int windowHeight)
{
	WindowWidth = windowWidth;
	WindowHeight = windowHeight;
	SettupInside(windowWidth, windowHeight);
	CurentHoldedWindow->SetSize(Width, Height);	
	for (size_t i = 0; i < CurentHoldedWindow->Viewports.Length(); i++)
		CurentHoldedWindow->Viewports[i]->SettupInside(Width, Height);
}

void QFAViewportHolder::ChangeWindow()
{
	if(CurentHoldedWindow == &HoldedWindow)
		CurentHoldedWindow = &HoldedWindowGame;
	else
		CurentHoldedWindow = &HoldedWindow;

	Settup(WindowWidth, WindowHeight);	
}

QFAViewportHolder::QFAViewportHolder()
{
	RegularViewport = false;
	HoldedWindow.Holder = this;
	HoldedWindowGame.Holder = this;
	QFAWindow::GameWindow = &HoldedWindowGame;
	CurentHoldedWindow = &HoldedWindow;
}

QFAViewportHolder::~QFAViewportHolder()
{
}

#endif