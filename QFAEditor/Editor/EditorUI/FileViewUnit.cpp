#include "epch.h"
#include "FileViewUnit.h"
#include <UI/Text.h>

QFAEditorUIFileViewUnit::QFAEditorUIFileViewUnit()
{
	Type = QFAUIType::CustomUnit;
	EditorType = QFAEditorUIType::FileViewUnit;
	Text = new QFAText;
	Text->SetTextSize(20);
	Text->SetOverflowWrap(QFAText::EOverflowWrap::OWNone);
	AddHiddenChild(Text);	
	Overflow = EOverflow::Hidden;
}

QFAEditorUIFileViewUnit::~QFAEditorUIFileViewUnit()
{
	delete Text;
}

void QFAEditorUIFileViewUnit::Enable(bool enable)
{
	IsEnable = enable;
	if (enable)
		Text->SetTextColor(QFAColorF(1.0f, 1.0f, 1.0f));
	else
		Text->SetTextColor(QFAColor(125, 125, 125));
} 

void QFAEditorUIFileViewUnit::SetText(std::u32string text)
{
	Text->SetText(text);
}

void QFAEditorUIFileViewUnit::ChangeSize(unsigned int w, unsigned int h)
{
	Width = w;
	Height = h;
	SetChildSize(Text, w, h);
}

void QFAEditorUIFileViewUnit::ChangePosition(int x, int y)
{
	Position_x = x;
	Position_y = y;
	SetChildPosition(Text, x, y);
}

float QFAEditorUIFileViewUnit::UpdateInnerHeight()
{
	return Height;
}

float QFAEditorUIFileViewUnit::UpdateInnerWidth()
{
	return Width;
}
