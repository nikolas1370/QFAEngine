#pragma once
#include <UI/UIParentMultipleUnit.h>

class QFAEngineViewport;
class QFAEngineWindow;
class QFAEXPORT QFAViewportRoot : public QFAUIParentMultipleUnit
{
	friend QFAEngineViewport;
	friend QFAEngineWindow;
	QFAEngineViewport* Viewport;

	void ChangeSize(unsigned int w, unsigned int h) final;
	void ChangePosition(int x, int y) final;

	void NewUnit(QFAUIUnit* unit) final;
	void MySlotChange(QFAUIUnit* unit) final;
	float UpdateInnerHeight() override;
	float UpdateInnerWidth() override;
public:
	QFAViewportRoot();
	~QFAViewportRoot();

	inline QFAEngineViewport* GetViewport()
	{
		return Viewport;
	}
};

