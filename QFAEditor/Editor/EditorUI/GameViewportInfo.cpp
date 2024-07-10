#include "epch.h"
#include "GameViewportInfo.h"
#include <UI/UIList.h>
#include <UI/Scroll.h>

#include <Object/Actor/Actor.h>
#include <UI/Canvas.h>

#include <EditorUI/UIActorTransform.h>
#include <UI/SelectUnit.h>
#include <UI/Text.h>
#include <filesystem>
#include <Tools/String.h>

QFAEditorGameViewportInfo::QFAEditorGameViewportInfo()
{	
	Canvas = new QFAUICanvas;
	AddHiddenChild(Canvas);

	QFAUISlot::SCanvasSlot slot;

	slot.x = 0.0f;
	slot.y = 0.0f;
	slot.Width = 1.0f;
	slot.Height = 0.5f;
	ActorList = new QFAUISelectList;
	ActorList->SetListType(QFAUIList::LTVertical);
	ActorList->SetUnitHeight(25);
	ActorList->FocusColor = InFocusUnitColor;
	ActorList->SelectColor = SelectUnit;
	ActorList->SelectLostFocusColor = SelectUnitNotFocus;
	ActorList->SelectEvent.LeftMouseDown = [this](QFAUIParent* unit)
		{
			if(!unit || unit == ActorList)
			{
				this->SelectActor(nullptr);
				return;
			}
			
			for (size_t i = 0; i < ActorAndTextList.size(); i++)
			{
				if (ActorAndTextList[i].text == unit)
				{
					SelectActor(ActorAndTextList[i].actor);
					return;
				}
			} 
		};

	ActorList->SetSlot(&slot);
	Canvas->AddUnit(ActorList);

	ActorInfoList = new QFAUIList;
	
	ActorTransform = new QFAUIActorTransform;
	ActorInfoList->AddUnit(ActorTransform);
	ActorInfoList->SetEnable(false);
	slot.y = 0.5f;
	ActorInfoList->SetSlot(&slot);
	Canvas->AddUnit(ActorInfoList);
}

QFAEditorGameViewportInfo::~QFAEditorGameViewportInfo()
{
	delete Canvas;
	delete ActorList;
	delete ActorTransform;
}

void QFAEditorGameViewportInfo::SelectActor(QActor* actor)
{
	if (!actor)
		return ActorList->SetSelectUnit(nullptr);
	
	for (size_t i = 0; i < ActorAndTextList.size(); i++)
	{
		if (ActorAndTextList[i].actor == actor)
		{
			ActorList->SetSelectUnit(ActorAndTextList[i].text);
			ActorInfoList->SetEnable(true);
			ActorTransform->SelectActor(actor);
			return;
		}
	}
}

void QFAEditorGameViewportInfo::AddActor(QActor* actor, std::u32string actorName, size_t id, bool isCppClass)
{
	QFATextBackground* text = new QFATextBackground;
	ActorList->AddUnit(text);
	text->SetTextSize(20);

	size_t count = 0;
	if(!isCppClass)
	{
		for (size_t i = 0; i < ActorTypes.size(); i++)
		{
			if (ActorTypes[i].fileId == id)
			{
				count = ++ActorTypes[i].count;
				break;
			}
		}
	}

	if (count == 0)
	{
		SActorTypes ats;
		ats.isCpp = isCppClass;
		ats.fileId = id;
		ActorTypes.push_back(ats);
	}
	else
		actorName.append(U"_").append(QFAString::NumToU32string(count));

	actor->Name = actorName;
	text->SetText(actorName);
	SActor sActor;
	sActor.actor = actor;
	sActor.text = text;
	ActorAndTextList.push_back(sActor);
}

void QFAEditorGameViewportInfo::PressedDelete()
{
	QFAUIParent* unit = ActorList->GetSelectedUnit();
	if (!unit || !ActorList->GetFocus())
		return;

	for (size_t i = 0; i < ActorAndTextList.size(); i++)
	{
		if (ActorAndTextList[i].text == unit)
		{
			ActorList->RemoveUnit(ActorAndTextList[i].text);
			delete ActorAndTextList[i].actor;
			delete ActorAndTextList[i].text;			
			ActorAndTextList.erase(ActorAndTextList.begin() + i);
			return;
		}
	}
}

void QFAEditorGameViewportInfo::MySlotChange(QFAUIUnit* unit)
{
}

void QFAEditorGameViewportInfo::ChangeSize(unsigned int w, unsigned int h)
{
	Width = w;
	Height = h;
	SetChildSize(Canvas, w, h);
}

void QFAEditorGameViewportInfo::ChangePosition(int x, int y)
{
	Position_x = x;
	Position_y = y;
	SetChildPosition(Canvas, Position_x, Position_y);
}

float QFAEditorGameViewportInfo::UpdateInnerHeight()
{
	return Height;
}

float QFAEditorGameViewportInfo::UpdateInnerWidth()
{
	return Width;
}
