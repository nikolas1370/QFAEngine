#pragma once
#include <Render/UI/UIParentMultipleUnit.h>

class QFAViewport;
class QFAWindow;
class QFAEXPORT QFAViewportRoot : public QFAUIParentMultipleUnit
{
	friend QFAViewport;
	friend QFAWindow;
	QFAViewport* Viewport;

	void ChangeSize(unsigned int w, unsigned int h) final;
	void ChangePosition(int x, int y) final;

	void NewUnit(QFAUIUnit* unit) final;
	void MySlotChange(QFAUIUnit* unit) final;
	float UpdateInnerHeight() override;
	float UpdateInnerWidth() override;
public:
	QFAViewportRoot();
	~QFAViewportRoot();

	inline QFAViewport* GetViewport()
	{
		return Viewport;
	}
};

