#include "UIActorList.h"
#include <EditorUI/UIActorList.h>
#include <Render/UI/UIList.h>
#include <Object/Actor/Actor.h>
#include <Render/UI/Background.h>
#include <Render/UI/Text.h>
#include <filesystem>

QFAEditorUIActorList::QFAEditorUIActorList(std::function<void(QActor*)> selectFunction)
{
	SelectFunction = selectFunction;
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

	actor->Name = name;
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
	QFAEditorUIActorList* thisUnit = (QFAEditorUIActorList*)_this;
	thisUnit->SelectFunction(nullptr);
	if (unit == _this)
	{
		thisUnit->CurentActorSelect = nullptr;
		return;
	}

	
	if (thisUnit->CurentActorSelect)
	{
		thisUnit->CurentActorSelect->SetBackgroundColor(thisUnit->OutFocusUnitColor);
		thisUnit->CurentActorSelect = nullptr;
	}

	QFAUIUnit* parent = unit;
	while (true)
	{
		if (_this == parent)
			return;

		if (parent->GetUnitType() == QFAUIType::Background)
		{
			for (size_t i = 0; i < thisUnit->ActorList.size(); i++)
			{
				if (thisUnit->ActorList[i].textBackgroundColor == parent)
				{
					thisUnit->SelectFunction(thisUnit->ActorList[i].actor);
					break;
				}
			}
			
			thisUnit->InputFocus = true;
			thisUnit->CurentActorSelect = (QFAUIBackground*)parent;
			thisUnit->CurentActorSelect->SetBackgroundColor(thisUnit->SelectUnit);
			return;
		}

		parent = parent->GetParent();
	}
}

void QFAEditorUIActorList::PressDelete()
{
	if (!CurentActorSelect || !InputFocus)
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

void QFAEditorUIActorList::SelectActor(QActor* actor)
{
	if (!actor->IsValid())
		return;

	for (size_t i = 0; i < ActorList.size(); i++)
	{
		if (ActorList[i].actor == actor) 
		{
			if (CurentActorSelect)
				CurentActorSelect->SetBackgroundColor(OutFocusUnitColor);

			InputFocus = true;
			CurentActorSelect = ActorList[i].textBackgroundColor;
			CurentActorSelect->SetBackgroundColor(SelectUnit);
			return;
		}
	}
}

void QFAEditorUIActorList::LostInputFocus()
{
	InputFocus = false;
	if(CurentActorSelect->IsValid())
		CurentActorSelect->SetBackgroundColor(SelectUnitNotFocus);
}
