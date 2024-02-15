#pragma once
#include <Render/UI/UIParentMultipleUnit.h>
#include <Math/Vector2D.h>

class QFAUICanvas : public QFAUIParentMultipleUnit
{


	
protected:
	
	void NewUnit(QFAUIUnit* unit) override;
	void MySlotChange(QFAUIUnit* unit) override;


	void ChangeSize(unsigned int w, unsigned int h) override;
	void ChangePosition(int x, int y) override;
	void UpdatePositionSizeChildren();

	float UpdateInnerHeight() override;
public:
	QFAUICanvas();
	~QFAUICanvas();



	

private:	
};
