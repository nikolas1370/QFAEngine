#pragma once
#include <Core/EngineStuff/Window/EngineWindow.h>


class QFAViewportHolder;
class QFAEngineViewport;
class QFAEXPORT QFAWindow : public QFAEngineWindow
{
	friend QFAViewportHolder;
	friend QFAEngineViewport;
	friend QFAEngineWindow;
protected:
#if QFA_EDITOR_ONLY
	QFAViewportHolder* Holder;
	static QFAWindow* GameWindow;
	QFAWindow();

#endif
public:
	static QFAWindow* GetWindow();
};
