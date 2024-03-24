#include "UIActorList.h"
#include <EditorUI/UIActorList.h>
#include <Render/UI/UIList.h>
#include <Object/Actor/Actor.h>
#include <Render/UI/Background.h>
#include <Render/UI/Text.h>
#include <filesystem>


QFAEditorUIActorList::QFAEditorUIActorList()
{
	UIActorList = new QFAUIList;
	UIActorList->SetListType(QFAUIList::LTVertical);
	UIActorList->SetUnitHeight(25);
	SetUnit(UIActorList);

	UIActorList->Events.SetInFocus(QFAEditorUIActorList::Infocus, this);
	UIActorList->Events.SetOutFocus(QFAEditorUIActorList::Outfocus, this);

	UIActorList->Events.SetLeftMouseDown(QFAEditorUIActorList::MouseDown, this);
}



QFAEditorUIActorList::~QFAEditorUIActorList()
{
}

void QFAEditorUIActorList::AddActor(QActor* actor, SEditorFile& ef)
{
	QFAUIBackground* color = new QFAUIBackground;
	UIActorList->AddUnit(color);
	QFAText* text = new QFAText;
	UIActorList->AddUnit(text);
	color->SetUnit(text);
	text->SetTextSize(20);

	size_t count = 0;
	for (size_t i = 0; i < ActorTypes.size(); i++)
	{
		if(ActorTypes[i].fileId == ef.id)
		{
			count = ++ActorTypes[i].count;
			break;
		}
	}

	std::u32string name = std::filesystem::path(ef.path).filename().replace_extension("").u32string();
	if (count == 0)
	{
		SActorTypes ats;
		ats.fileId = ef.id;
		ActorTypes.push_back(ats);
	}
	else
		name.append(U"_").append(NumToU32string(count));

	text->SetText(name);
	SActor sActor;
	sActor.actor = actor;
	sActor.textBackgroundColor = color;
	ActorList.push_back(sActor);
}


void QFAEditorUIActorList::Infocus(QFAUIUnit* unit, void* _this)
{
	QFAEditorUIActorList* thisUnit = (QFAEditorUIActorList*)_this;

	if (unit == _this && !thisUnit->CurentActorSelect)
	{
		thisUnit->CurentActorInFocus->SetBackgroundColor(thisUnit->OutFocusUnitColor);
		thisUnit->CurentActorInFocus = nullptr;
		return;
	}

	if (thisUnit->CurentActorInFocus && thisUnit->CurentActorInFocus != thisUnit->CurentActorSelect)
		thisUnit->CurentActorInFocus->SetBackgroundColor(thisUnit->OutFocusUnitColor);

	QFAUIUnit* parent = unit;
	while (true)
	{
		if (_this == parent)
			return;
		
		if (parent->GetUnitType() == QFAUIType::Background)
		{
			if(thisUnit->CurentActorSelect != parent)
			{
				thisUnit->CurentActorInFocus = (QFAUIBackground*)parent;
				thisUnit->CurentActorInFocus->SetBackgroundColor(thisUnit->InFocusUnitColor);
			}

			return;
		}

		parent = parent->GetParent();
	}
}

void QFAEditorUIActorList::Outfocus(void* _this)
{
	QFAEditorUIActorList* thisUnit = (QFAEditorUIActorList*)_this;
	if (thisUnit->CurentActorInFocus )
	{		
		if(thisUnit->CurentActorInFocus != thisUnit->CurentActorSelect)
			thisUnit->CurentActorInFocus->SetBackgroundColor(thisUnit->OutFocusUnitColor);

		thisUnit->CurentActorInFocus = nullptr;
	}
}

void QFAEditorUIActorList::MouseDown(QFAUIUnit* unit, void* _this)
{
	if (unit == _this)
		return;

	QFAEditorUIActorList* thisUnit = (QFAEditorUIActorList*)_this;
	if (thisUnit->CurentActorSelect)
		thisUnit->CurentActorSelect->SetBackgroundColor(thisUnit->OutFocusUnitColor);

	QFAUIUnit* parent = unit;
	while (true)
	{
		if (_this == parent)
			return;

		if (parent->GetUnitType() == QFAUIType::Background)
		{
			thisUnit->CurentActorSelect = (QFAUIBackground*)parent;
			thisUnit->CurentActorSelect->SetBackgroundColor(thisUnit->SelectUnit);
			if(thisUnit->SelectFun)
				thisUnit->SelectFun();

			return;
		}

		parent = parent->GetParent();
	}
}

void QFAEditorUIActorList::PressDelete()
{
	if (!CurentActorSelect)
		return;

	for (size_t i = 0; i < ActorList.size(); i++)
	{
		if (ActorList[i].textBackgroundColor == CurentActorSelect)
		{
			UIActorList->removeUnit(ActorList[i].textBackgroundColor);
			delete ActorList[i].actor;
			delete ActorList[i].textBackgroundColor;
			ActorList.erase(ActorList.begin() + i);
			return;
		}
	}
	
}

std::u32string QFAEditorUIActorList::NumToU32string(size_t num)
{
 	std::string str = std::to_string(num);

	std::u32string bigString;
	bigString.resize(str.size());
	for (size_t i = 0; i < bigString.size(); i++)
		bigString[i] = (char32_t)str[i];

	return bigString;
}
