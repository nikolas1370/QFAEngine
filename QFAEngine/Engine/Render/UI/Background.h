#pragma once
#include <Render/UI/UIParentOneUnit.h>

class QFAUIBackground : public QFAUIParentOneUnit
{

	void MySlotChange(QFAUIUnit* unit) override;


	void ChangeSize(unsigned int w, unsigned int h) override;

	void ChangePosition(int x, int y) override;

	float UpdateInnerHeight() override;
	float UpdateInnerWidth() override;
	void ChangedUnit(QFAUIUnit* unit) override;
public:
	QFAUIBackground();
	~QFAUIBackground();

private:

};
