#pragma once
#include <string>
#include <UI/UIParentHiddenChild.h>

class QFAText;
class QFAEditorUIFileViewUnit : public QFAParentHiddenChild
{
	bool IsEnable = false;
	QFAText* Text = nullptr;

protected:
	void MySlotChange(QFAUIUnit* unit) override {}
	float UpdateInnerHeight() override;
	float UpdateInnerWidth() override;

	void WidthChanged(int oldValue = 0) override;
	void HeightChanged(int oldValue = 0) override;
	void TopChanged(int oldValue = 0) override;
	void LeftChanged(int oldValue = 0) override;

public:
	QFAEditorUIFileViewUnit();
	~QFAEditorUIFileViewUnit();

	void Enable(bool enable);
	void SetText(std::u32string text);
};