#pragma once
#include <Render/UI/UIParentHiddenChild.h>
#include <Render/UI/Text.h>

class QFAUIEvent;
class QFAUIScroll;
class QFAUITextInput : public QFAParentHiddenChild
{
	friend QFAUIEvent;
	friend QFAUIScroll;
	static const int MaxTextCount = 256;
	QFAText Text;
	char32_t Texts[MaxTextCount];
	QFAText::PrepareData pd[MaxTextCount];
public:
	QFAUITextInput();
	~QFAUITextInput();

	inline bool SetFont(QFAText::SFont* font)
	{
		return Text.SetFont(font);
	}
protected:
	inline QFAVKPipeline* GetChildPipeline() override
	{
		return QFAText::Pipeline;
	}

	inline void RenderChild(VkCommandBuffer comandebuffer) override
	{
		Text.Render(comandebuffer);
	}
	
	inline QFAUIUnit* GetChild() override
	{
		return &Text;
	}

	void MySlotChange(QFAUIUnit* unit) override {};

	void ChangeSize(unsigned int w, unsigned int h) override;
	void ChangePosition(int x, int y) override;
	float UpdateInnerHeight() override;
	float UpdateInnerWidth() override;

	void InInputfocus(unsigned int cursorX, unsigned int cursorY);
	void OutInputfocus();
	void AddChar(unsigned int charCode);
	void RemoveChar();



	void PenLeft();
	void PenRight();

	void UpdateUnitOffset();

	void NewFrame(float delta);

	// pen blink
	const float MaxPenTime = 0.5f; // static
	float PenTime = 0.0f;

	//Create Getvalue
};