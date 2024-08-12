#include "pch.h"
#if QFA_EDITOR_ONLY
#include "ViewportHolder.h"

void QFAViewportHolder::Settup(int windowWidth, int windowHeight)
{
	HoldedWindow.InitNotRegularWindow(Window);
	HoldedWindowGame.InitNotRegularWindow(Window);

	WindowWidth = windowWidth;
	WindowHeight = windowHeight;
	SettupInside(windowWidth, windowHeight);
	CurentHoldedWindow->SetSize(Width, Height);	
	for (size_t i = 0; i < CurentHoldedWindow->Viewports.size(); i++)
		CurentHoldedWindow->Viewports[i]->SettupInside(Width, Height);
}

void QFAViewportHolder::ChangeWindow()
{
	if(CurentHoldedWindow == &HoldedWindowGame)
		CurentHoldedWindow = &HoldedWindow;
	else
		CurentHoldedWindow = &HoldedWindowGame;
		
	Settup(WindowWidth, WindowHeight);	
}

QFAViewportHolder::QFAViewportHolder()
{	
	RegularViewport = false;
	HoldedWindow.Holder = this;
	HoldedWindowGame.Holder = this;
	CurentHoldedWindow = &HoldedWindow;
	QFAWindow::GameWindow = &HoldedWindowGame;
}

QFAViewportHolder::~QFAViewportHolder()
{
}

#endif