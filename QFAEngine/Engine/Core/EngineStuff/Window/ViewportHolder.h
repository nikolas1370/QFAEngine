#pragma once
#if QFA_EDITOR_ONLY
#include <Core/Window/Window.h>
#include <Tools/Stuff.h>
#include <Core/Window/Viewport.h>

/*
	this viewport hold window
	all viewport inside Window be render and procesTick if QFAViewportHolder attached to QFAEngineWindow
*/
class QFAViewport;
class QFAEXPORT QFAViewportHolder : public QFAViewport
{
	friend QFAEngineWindow;
	friend QFAViewport;
protected:
	int windowWidth;
	int windowHeight;
	QFAWindow HoldedWindow;
	QFAWindow HoldedWindowGame;
	QFAWindow* CurentHoldedWindow = nullptr;
	void Settup(int windowWidth, int windowHeight) final;

	void ChangeWindow();;
public:
	QFAViewportHolder();
	~QFAViewportHolder();
};

#endif