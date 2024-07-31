#pragma once
#include <UI/UIParentMultipleUnit.h>

class QFAEngineViewport;
class QFAEngineWindow;
class QFAEXPORT QFAViewportRoot : public QFAUIParentMultipleUnit
{
	friend QFAEngineViewport;
	friend QFAEngineWindow;
	QFAEngineViewport* Viewport;



	void NewUnit(QFAUIUnit* unit) final;
	void MySlotChange(QFAUIUnit* unit) final;
	float UpdateInnerHeight() override;
	float UpdateInnerWidth() override;

protected:
	void WidthChanged(int oldValue = 0) override;
	void HeightChanged(int oldValue = 0) override;
	void TopChanged(int oldValue = 0) override;
	void LeftChanged(int oldValue = 0) override;

public:
	QFAViewportRoot();
	~QFAViewportRoot();



	inline QFAEngineViewport* GetViewport()
	{
		return Viewport;
	}
};

