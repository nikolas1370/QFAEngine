#pragma once
#include <string>
#include <Render/UI/UIParentHiddenChild.h>

class QFAText;
class QFAEditorUIFileViewUnit : public QFAParentHiddenChild
{
	bool IsEnable = false;
	QFAText* Text;
public:
	QFAEditorUIFileViewUnit();
	~QFAEditorUIFileViewUnit();
	void Enable(bool enable);
	void SetText(std::u32string text);

protected:
	void MySlotChange(QFAUIUnit* unit) override
	{
	}
	void ChangeSize(unsigned int w, unsigned int h) override;
	void ChangePosition(int x, int y) override;
	float UpdateInnerHeight() override;
	float UpdateInnerWidth() override;
};