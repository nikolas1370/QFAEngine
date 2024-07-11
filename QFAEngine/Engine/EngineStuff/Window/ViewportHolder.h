#pragma once
#if QFA_EDITOR_ONLY
#include <Window/Window.h>
#include <Tools/Stuff.h>
#include <Window/Viewport.h>

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
	QFAWindow HoldedWindow;
	void Settup(int windowWidth, int windowHeight) final;
public:
	QFAViewportHolder();
	~QFAViewportHolder();
};

#endif