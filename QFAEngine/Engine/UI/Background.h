#pragma once
#include <UI/UIParentOneUnit.h>

/*
	put inside text, input, etc and set backroung color
*/
class QFAEXPORT QFAUIBackground : public QFAUIParentOneUnit
{
	void MySlotChange(QFAUIUnit* unit) override;
	/*
	void ChangeSize(unsigned int w, unsigned int h) override;
	void ChangePosition(int x, int y) override;
	
	*/

	float UpdateInnerHeight() override;
	float UpdateInnerWidth() override;
	void ChangedUnit(QFAUIUnit* unit) override;


protected:
	void WidthChanged(int oldValue = 0) override;
	void HeightChanged(int oldValue = 0) override;
	void TopChanged(int oldValue = 0) override;
	void LeftChanged(int oldValue = 0) override;

public:
	QFAUIBackground();
	~QFAUIBackground();




};
