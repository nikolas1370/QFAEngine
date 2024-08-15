#pragma once
#include <UI/UIParentHiddenChild.h>
#include <UI/Text.h>

class QFAUIEvent;
class QFAUIScroll;
class QFAEXPORT QFAUITextInput : public QFAParentHiddenChild
{
	friend QFAUIEvent;
	friend QFAUIScroll;
	friend QFAText;


public:
	enum ENumberType
	{
		None,
		Int,
		Float
	};

private:
	static const int MaxTextCount = 256;
protected:
	// pen blink
	const float MaxPenTime = 0.5f; // static

private:
	static char convertStr[21];
	QFAText* Text;
	char32_t Texts[MaxTextCount];
	QFAText::PrepareData pd[MaxTextCount];
	
	float  ValidFLoat = 0.0f;
	int  ValidInt = 0;
	ENumberType NumberType;
	std::function<void()> TextChange;
protected:
	std::function<void(QFAUITextInput*)> OutFocusFun;
	
	float PenTime = 0.0f;

	void MySlotChange(QFAUIUnit* unit) override {};

	float UpdateInnerHeight() override;
	float UpdateInnerWidth() override;

	void InInputfocus(unsigned int cursorX, unsigned int cursorY);
	void OutInputfocus();
	void AddChar(unsigned int charCode);
	void RemoveChar();



	void PenLeft();
	void PenRight();

	void UpdateUnitOffset(bool removeChar = false);

	void NewFrame(float delta);

	void ChildInnerChange(QFAUIUnit* child) override;

public:
	QFAUITextInput(ENumberType numberT = ENumberType::None);
	~QFAUITextInput();

	void WidthChanged(int oldValue = 0) override;
	void HeightChanged(int oldValue = 0) override;
	void TopChanged(int oldValue = 0) override;
	void LeftChanged(int oldValue = 0) override;

	inline bool SetFont(QFAText::SFont* font)
	{
		return Text->SetFont(font);
	}

	inline void SetTextSize(size_t size)
	{
		Text->SetTextSize(size);
	}
	inline void SetTextColor(QFAColor color)
	{
		Text->SetTextColor(color);
	}

	inline void SetTextAlign(QFAText::ETextAlign aligh)
	{
		Text->SetTextAlign(aligh);
	}

	std::u32string GetValue();
	void SetValue(float value);
	void SetValue(int value);
	void SetValue(std::u32string value);

	void SetOutFocusFun(std::function<void(QFAUITextInput*)> fun);
	// if text was change fun be call
	void SetTextChange(std::function<void()> fun);
};