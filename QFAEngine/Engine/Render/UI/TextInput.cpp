#include "TextInput.h"

QFAUITextInput::QFAUITextInput()
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
	Children.push_back(Text);
}

QFAUITextInput::~QFAUITextInput()
{
	Text->Parent = nullptr;
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
}

void QFAUITextInput::AddChar(unsigned int charCode)
{
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

	UpdateUnitOffset();
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


void QFAUITextInput::UpdateUnitOffset()
{	
	QFAText::GlyphShader gs;
	Text->GetPenPosition(gs);
	if (gs.leftTop_1.x + Text->UnitOffsetX < Position_x) // left
		Text->UnitOffsetX = Position_x - gs.leftTop_1.x; // + value
	else if(gs.rightTop_2.x + Text->UnitOffsetX > Position_x + Width) // right
		Text->UnitOffsetX = Position_x + Width - gs.rightTop_2.x; // - value

	if (gs.leftTop_1.y + Text->UnitScroll < Position_y) // up
		Text->UnitScroll = Position_y - gs.leftTop_1.y; // + value
	else if(gs.leftBottom_1.y + Text->UnitScroll > Position_y + Height) // down
		Text->UnitScroll = Position_y + Height - gs.leftBottom_1.y; // - value
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


