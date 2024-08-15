#include "epch.h"
#include "FileViewUnit.h"
#include <UI/Text.h>
#include <Tools/String.h>
#include <EngineClassesInterface.h>

QFAEditorUIFileViewUnit::QFAEditorUIFileViewUnit()
{
	Type = QFAUIType::CustomUnit;
	EditorType = QFAEditorUIType::FileViewUnit;
	Text = NewUI<QFAText>();
	Text->SetTextSize(20);
	Text->SetOverflowWrap(QFAText::EOverflowWrap::OWNone);
	AddHiddenChild(Text);	
	Overflow = EOverflow::Hidden;
}

QFAEditorUIFileViewUnit::~QFAEditorUIFileViewUnit()
{
	Text->Destroy();
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

void QFAEditorUIFileViewUnit::WidthChanged(int oldValue)
{
	((QFAEditorText*)Text)->ParentSetWidth = Width;
	Text->SetWidth(((QFAEditorText*)Text)->StrWidth, ((QFAEditorText*)Text)->ParentSetWidthMinus);
}

void QFAEditorUIFileViewUnit::HeightChanged(int oldValue)
{
	((QFAEditorText*)Text)->ParentSetHeight = Height;
	Text->SetHeight(((QFAEditorText*)Text)->StrHeight, ((QFAEditorText*)Text)->ParentSetHeightMinus);
}

void QFAEditorUIFileViewUnit::TopChanged(int oldValue)
{
	((QFAEditorText*)Text)->ParentSetPosition_y = Position_y;
	Text->SetTop(((QFAEditorText*)Text)->StrTop);
}

void QFAEditorUIFileViewUnit::LeftChanged(int oldValue)
{
	((QFAEditorText*)Text)->ParentSetPosition_x = Position_x;
	Text->SetLeft(((QFAEditorText*)Text)->StrLeft);
}

float QFAEditorUIFileViewUnit::UpdateInnerHeight()
{
	return Height;
}

float QFAEditorUIFileViewUnit::UpdateInnerWidth()
{
	return Width;
}

