#include "epch.h"
#include "ExplorerFolderUnit.h"
#include <UI/Canvas.h>
#include  <UI/UIImage.h>
#include <UI/Text.h>
#include <EngineClassesInterface.h>
#include <Tools/String.h>

QFAImage* QFAEditorExplorerFolderUnit::FolderImage = nullptr;
QFAImage* QFAEditorExplorerFolderUnit::FileImage = nullptr;

QFAEditorExplorerFolderUnit::QFAEditorExplorerFolderUnit()
{
	Type = QFAUIType::CustomUnit;
	EditorType = QFAEditorUIType::ExplorerFolderUnit;
	Canvas = NewUI<QFAUICanvas>();
	Canvas->SetOverflow(QFAUIParent::Hidden);
	AddHiddenChild(Canvas);	

	if (!FolderImage)
	{
		FolderImage = QFAEditorImage::CreateImage(U"SomeImage/folder-png-download-png-1024px-1024.png");
		FileImage = QFAEditorImage::CreateImage(U"SomeImage/file-text-icon-1360881626.png");
	}

	Icon = NewUI<QFAUIImage>(nullptr);
	UnitText = NewUI<QFAText>();
	UnitText->SetTextSize(16);
	UnitText->SetTextAlign(QFAText::TACenter);
	UnitText->SetOverflowWrap(QFAText::OWWordBreak);
	Canvas->AddUnit(Icon);
	Canvas->AddUnit(UnitText);
	
	Icon->SetWidth("100%");
	Icon->SetHeight("50%");
	
	UnitText->SetWidth("100%");
	UnitText->SetHeight("50%");
	UnitText->SetTop("50%");
}

QFAEditorExplorerFolderUnit::~QFAEditorExplorerFolderUnit()
{
	UnitText->Destroy();
	Icon->Destroy();
	Canvas->Destroy();
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

void QFAEditorExplorerFolderUnit::WidthChanged(int oldValue)
{
	((QFAEditorCanvas*)Canvas)->ParentSetWidth = Width;
	Canvas->SetWidth("100%", false);
}

void QFAEditorExplorerFolderUnit::HeightChanged(int oldValue)
{
	((QFAEditorCanvas*)Canvas)->ParentSetHeight = Height;
	Canvas->SetHeight("100%", false);
}

void QFAEditorExplorerFolderUnit::TopChanged(int oldValue)
{
	((QFAEditorCanvas*)Canvas)->ParentSetPosition_y = Position_y;
	Canvas->SetTop(nullptr);
}

void QFAEditorExplorerFolderUnit::LeftChanged(int oldValue)
{
	((QFAEditorCanvas*)Canvas)->ParentSetPosition_x = Position_x;
	Canvas->SetLeft(nullptr);
}
