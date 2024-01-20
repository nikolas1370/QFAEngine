#pragma once
#include <Render/UI/UIParentComponent.h>


class QFAUICanvas : public QFAUIParentComponent
{


	void ViewportSizeChange(int viewportWidth, int viewportHeight) override;
protected:
	
	void NewUnit(QFAUIUnit* unit) override;
	void MySlotChange(QFAUIUnit* unit) override;


	void SetSizeParent(unsigned int w, unsigned int h) override {}
	void SetPositionParent(unsigned int x, unsigned int y) override {}
	
public:
	QFAUICanvas();
	~QFAUICanvas();



	

private:	
};
