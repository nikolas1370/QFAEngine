#include "pch.h"
#include "TextInput.h"
#include "Render/Buffer/VertexBuffer.h"

char QFAUITextInput::convertStr[21];

QFAUITextInput::QFAUITextInput(ENumberType numberT)
{
	Type = QFAUIType::TextInput;
	Text = new QFAText;

	Text->Parent = this;
	Text->Text.pen = 0;
	Text->Text.penEnable = true;

	Text->SetInputText(&Texts[0], 0, MaxTextCount);
	Text->TextMetadata.onlyText = false;
	Text->TextMetadata.inputData = &pd[0];
	Overflow = EOverflow::Hidden; 	
	AddHiddenChild(Text);	
	NumberType = numberT;	
	if (NumberType == ENumberType::Float)
		SetValue(0.0f);
	else if (NumberType == ENumberType::Int)
		SetValue(0);
	
}

QFAUITextInput::~QFAUITextInput()
{
	Text->Parent = nullptr;
}

std::u32string QFAUITextInput::GetValue()
{
	std::u32string val;
	val.resize(Text->Text.size());
	for (size_t i = 0; i < Text->Text.size(); i++)
		val[i] = Text->Text.pText[i];

	return val;
}

void QFAUITextInput::SetValue(float value)
{
	std::string str = std::to_string(value);
	for (size_t i = 0; i < str.size(); i++)
		Text->Text.pText[i] = (char32_t)str[i];

	
	Text->Text.pTextSize = str.size();
	Text->Text.pen = 0;
	UpdateUnitOffset();
	Text->TextChange = true;
}

void QFAUITextInput::SetValue(int value)
{
	std::string str = std::to_string(value);
	for (size_t i = 0; i < str.size(); i++)
		Text->Text.pText[i] = (char32_t)str[i];

	Text->Text.pTextSize = str.size();
	Text->Text.pen = 0;
	UpdateUnitOffset();
	Text->TextChange = true;
}

void QFAUITextInput::SetValue(std::u32string value)
{
	int maxText = value.size() > MaxTextCount ? MaxTextCount : value.size();
	for (size_t i = 0; i < maxText; i++)
		Text->Text.pText[i] = value[i];

	Text->Text.pTextSize = maxText;
	Text->Text.pen = 0;
	UpdateUnitOffset();
	Text->TextChange = true;
}

void QFAUITextInput::SetOutFocusFun(std::function<void(QFAUITextInput*)> fun)
{
	OutFocusFun = fun;
}

void QFAUITextInput::ChangeSize(unsigned int w, unsigned int h)
{
	Width = w;
	Height = h;
	Text->SetSizeParent(w, h);
}

void QFAUITextInput::ChangePosition(int x, int y)
{
	Position_x = x;
	Position_y = y;	
	Text->SetPositionParent(x, y);
}

float QFAUITextInput::UpdateInnerHeight()
{
	return Text->InnerHeight + 10; // + 10 for pen height if not in scroll be problem at last row
}

float QFAUITextInput::UpdateInnerWidth()
{
	InnerWidth = Text->InnerWidth;
	return InnerWidth;
}

void QFAUITextInput::InInputfocus(unsigned int cursorX, unsigned int cursorY)
{
	PenTime = MaxPenTime;
	Text->Text.CanSeePen = true;
	cursorX -= Text->UnitOffsetX;
	cursorY -= Text->UnitScroll;

	Text->Text.inputFocus = true;

	if (NumberType == ENumberType::Float)
	{
		memset(convertStr, 0, sizeof(convertStr));
		for (size_t i = 0; i < sizeof(convertStr) - 1; i++)
			convertStr[i] = (char)Text->Text.pText[i];
		try
		{
			ValidFLoat = std::stoi(convertStr);
		}
		catch (const std::exception&)
		{

		}
	}
	else if (NumberType == ENumberType::Int)
	{
		memset(convertStr, 0, sizeof(convertStr));
		for (size_t i = 0; i < sizeof(convertStr) - 1; i++)
			convertStr[i] = (char)Text->Text.pText[i];
		try
		{
			ValidInt = std::stoi(convertStr);
		}
		catch (const std::exception&)
		{

		}
	}
	

	QFAText::GlyphShader *gs = (QFAText::GlyphShader*)Text->vertexBufer->CpuSideBuffer->MapData;
	int index = 0;
	for (size_t i = 0; i < Text->Text.pTextSize; i++)
	{
		if (cursorX >= gs[i].leftTop_1.x && cursorX <= gs[i].rightBottom_1.x  &&
			cursorY >= gs[i].leftTop_1.y && cursorY <= gs[i].rightBottom_1.y )
		{
			if (i + 1 < Text->Text.pTextSize)
			{
				if (cursorX >= gs[i + 1].leftTop_1.x && cursorX <= gs[i + 1].rightBottom_1.x &&
					cursorY >= gs[i + 1].leftTop_1.y && cursorY <= gs[i + 1].rightBottom_1.y)
				{ // if symbol[i] and symbol[i + 1] hit cursor compare who closer to cursor
					int fist = abs(gs[i].rightBottom_1.x - cursorX);
					int second = abs(gs[i + 1].leftTop_1.x - cursorX);
					if (fist < second) // second closer to cursor
						i++;
				}
			}
			
			Text->Text.pen = i;
			UpdateUnitOffset();
			return;
		}
	}
	
	Text->Text.pen = Text->Text.pTextSize;
	UpdateUnitOffset();
}

void QFAUITextInput::OutInputfocus()
{
	Text->Text.inputFocus = false;
	PenTime = 0.0f;

	if (NumberType == ENumberType::Float)
	{
		memset(convertStr, 0, sizeof(convertStr));
		for (size_t i = 0; i < sizeof(convertStr) - 1; i++)
			convertStr[i] = (char)Text->Text.pText[i];

		try
		{
			ValidFLoat = std::stof(convertStr);
		}
		catch (const std::exception&)
		{
				
		}

		Text->Text.pTextSize = 0;
		Text->Text.pen = 0;
		std::string str = std::to_string(ValidFLoat);
		for (size_t i = 0; i < str.size(); i++)
			AddChar((unsigned int)str[i]);

		Text->Text.pen = 0;
		UpdateUnitOffset();
	}
	else if (NumberType == ENumberType::Int)
	{		
		try
		{
			ValidInt = std::stoi(convertStr);
		}
		catch (const std::exception&)
		{
			
		}

		Text->Text.pTextSize = 0;
		Text->Text.pen = 0;
		std::string str = std::to_string(ValidInt);
		for (size_t i = 0; i < str.size(); i++)
			AddChar((unsigned int)str[i]);

		Text->Text.pen = 0;
		UpdateUnitOffset();
	}

	if (OutFocusFun)
		OutFocusFun(this);
}

void QFAUITextInput::AddChar(unsigned int charCode)
{
	if ((NumberType == ENumberType::Float || NumberType == ENumberType::Int) &&
		!(charCode >= (unsigned int)'0' && charCode <= (unsigned int)'9' || charCode == (unsigned int)'.' || charCode == (unsigned int)'-') )
		return;		

	if (Text->Text.pTextSize >= MaxTextCount)
		return;

	for (size_t i = Text->Text.pTextSize; i > Text->Text.pen; i--)
		Texts[i] = Texts[i - 1];	

	Texts[Text->Text.pen] = charCode;
	Text->Text.pen++;
	Text->Text.pTextSize++;	
	Text->ProcessText(); // before UpdateUnitOffset need re proces text, otherwise GetPenPosition return error value
	UpdateUnitOffset();
		
	


	PenTime = MaxPenTime;
	Text->Text.CanSeePen = true;
}

void QFAUITextInput::RemoveChar()
{
	if (Text->Text.pen == 0)
		return;

	for (size_t i = Text->Text.pen; i < Text->Text.pTextSize; i++)
		Texts[i - 1] = Texts[i];
	
	Text->Text.pen--;
	Text->Text.pTextSize--;
	if (Text->Text.pTextSize > 0)
		Text->ProcessText(); // before UpdateUnitOffset need re proces text, otherwise GetPenPosition return error value
	else
	{
		Text->CountSymbolForRender = 0;
		Text->TextMetadata.Clear();
	}

	
	UpdateUnitOffset(true);
	PenTime = MaxPenTime;
	Text->Text.CanSeePen = true;
}

void QFAUITextInput::PenLeft()
{
	if (Text->Text.pen > 0)
	{
		Text->Text.pen--;
		UpdateUnitOffset();
		PenTime = MaxPenTime;
		Text->Text.CanSeePen = true;
	}
}

void QFAUITextInput::PenRight()
{
	if (Text->Text.pen < Text->Text.pTextSize && Text->Text.pen < MaxTextCount)
	{
		Text->Text.pen++;
		UpdateUnitOffset();
		PenTime = MaxPenTime;
		Text->Text.CanSeePen = true;
	}
}


void QFAUITextInput::UpdateUnitOffset(bool removeChar)
{	
	QFAText::GlyphShader gs;
	Text->GetPenPosition(gs);
	if (gs.leftTop_1.x <= 0) // left
	{
		/*
			if type left set 0
			in other some else
			Text->UnitOffsetX = gs.leftTop_1.x;
		*/
		Text->UnitOffsetX = 1;
	}	
	else if (gs.rightTop_2.x > Width - Text->UnitOffsetX ||
			(removeChar && Text->Text.pen == Text->Text.pTextSize && Text->UnitOffsetX < 0)) // right
	{
		Text->UnitOffsetX = Width - gs.rightTop_2.x;		
	}
	else if (gs.rightTop_2.x < -Text->UnitOffsetX)// left
		Text->UnitOffsetX = -gs.leftTop_1.x;

	if (gs.leftTop_1.y  < 0) // up
		Text->UnitScroll = gs.leftTop_1.y; 
	else if (gs.leftBottom_1.y > Height - Text->UnitScroll) // down
		Text->UnitScroll = Height - gs.leftBottom_1.y;
	else if(gs.leftBottom_1.y < -Text->UnitScroll)// up
		Text->UnitScroll = -Text->UnitScroll;
}

void QFAUITextInput::NewFrame(float delta)
{
	PenTime -= delta;
	if (PenTime <= 0.0f)
	{
		PenTime = MaxPenTime;
		Text->Text.CanSeePen = !Text->Text.CanSeePen;
	}
}

void QFAUITextInput::ChildInnerChange(QFAUIUnit* child)
{
	if (Parent->IsValid())
		Parent->ChildInnerChange(this);
}


