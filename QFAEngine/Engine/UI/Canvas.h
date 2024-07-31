#pragma once
#include <UI/UIParentMultipleUnit.h>
#include <Math/Vector2D.h>

class QFAEXPORT QFAUICanvas : public QFAUIParentMultipleUnit
{
protected:	
	void NewUnit(QFAUIUnit* unit) override;
	void MySlotChange(QFAUIUnit* unit) override;

	float UpdateInnerHeight() override;
	float UpdateInnerWidth() override;

	void WidthChanged(int oldValue = 0) override;
	void HeightChanged(int oldValue = 0) override;
	void TopChanged(int oldValue = 0) override;
	void LeftChanged(int oldValue = 0) override;

public:
	QFAUICanvas();
	~QFAUICanvas();
};