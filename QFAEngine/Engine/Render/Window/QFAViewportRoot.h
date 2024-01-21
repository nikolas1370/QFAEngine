#pragma once
#include <Render/UI/UIParentComponent.h>

class QFAViewport;
class QFAWindow;
class QFAViewportRoot : public QFAUIParentComponent
{
	friend QFAViewport;
	friend QFAWindow;
	QFAViewport* Viewport;

	void SetSizeParent(unsigned int w, unsigned int h) final;
	void SetPositionParent(unsigned int x, unsigned int y) final;

	void NewUnit(QFAUIUnit* unit) final;
	void MySlotChange(QFAUIUnit* unit) final;
protected:
	
public:
	QFAViewportRoot();
	~QFAViewportRoot();


private:

};

