#include "epch.h"
#include "ExplorerFolderUnit.h"
#include <UI/Canvas.h>
#include  <UI/UIImage.h>
#include <UI/Text.h>
#include <EngineClassesInterface.h>

QFAImage* QFAEditorExplorerFolderUnit::FolderImage = nullptr;
QFAImage* QFAEditorExplorerFolderUnit::FileImage = nullptr;

QFAEditorExplorerFolderUnit::QFAEditorExplorerFolderUnit()
{
	Type = QFAUIType::CustomUnit;
	EditorType = QFAEditorUIType::ExplorerFolderUnit;
	Canvas = new QFAUICanvas;
	Canvas->SetOverflow(QFAUIParent::Hidden);
	AddHiddenChild(Canvas);	

	if (!FolderImage)
	{
		FolderImage = QFAEditorImage::CreateImage(U"SomeImage/folder-png-download-png-1024px-1024.png");
		FileImage = QFAEditorImage::CreateImage(U"SomeImage/file-text-icon-1360881626.png");
	}

	Icon = new QFAUIImage(nullptr);
	UnitText = new QFAText;
	UnitText->SetTextSize(16);
	UnitText->SetTextAlign(QFAText::TACenter);
	UnitText->SetOverflowWrap(QFAText::OWWordBreak);
	Canvas->AddUnit(Icon);
	Canvas->AddUnit(UnitText);
	
	QFAUISlot::SCanvasSlot slot;
	slot.Height = 0.5f;
	slot.Width = 1.0f;
	slot.x = 0.0f;
	slot.y = 0.0f;
	Icon->SetSlot(&slot);

	slot.Height = 0.5f;
	slot.Width = 1.0f;
	slot.x = 0.0f;
	slot.y = 0.5f;
	UnitText->SetSlot(&slot);	
}

QFAEditorExplorerFolderUnit::~QFAEditorExplorerFolderUnit()
{
}

void QFAEditorExplorerFolderUnit::MySlotChange(QFAUIUnit* unit)
{
}

float QFAEditorExplorerFolderUnit::UpdateInnerHeight()
{
	return Height;
}

float QFAEditorExplorerFolderUnit::UpdateInnerWidth()
{
	return Width;
}

void QFAEditorExplorerFolderUnit::ChangeImage(bool foldar)
{
	Icon->SetImage(foldar ? FolderImage : FileImage);
}

void QFAEditorExplorerFolderUnit::ChangeText(std::u32string text)
{
	UnitText->SetText(text);
}

void QFAEditorExplorerFolderUnit::ChangeSize(unsigned int w, unsigned int h)
{
	SetChildSize(Canvas, w, h);	
}


void QFAEditorExplorerFolderUnit::ChangePosition(int x, int y)
{
	SetChildPosition(Canvas, x, y);
}