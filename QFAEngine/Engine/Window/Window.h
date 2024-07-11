#pragma once
#include <EngineStuff/Window/EngineWindow.h>


class QFAViewportHolder;
class QFAViewport;
class QFAEXPORT QFAWindow : public QFAEngineWindow
{
	friend QFAViewportHolder;
	friend QFAViewport;
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
