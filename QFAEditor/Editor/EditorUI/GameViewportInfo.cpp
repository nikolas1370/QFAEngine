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
#include <EngineClassesInterface.h>

QFAEditorGameViewportInfo::QFAEditorGameViewportInfo()
{	
	Canvas = NewUI<QFAUICanvas>();
	AddHiddenChild(Canvas);

	ActorList = NewUI<QFAUISelectList>();
	ActorList->SetListType(QFAUIList::LTVertical);
	ActorList->SetUnitHeight(25);
	ActorList->FocusColor = InFocusUnitColor;
	ActorList->SelectColor = SelectUnit;
	ActorList->SelectLostFocusColor = SelectUnitNotFocus;
	ActorList->SelectEvent.LeftMouseDown = [this](QFAUIParent* unit, void* extra)
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

	ActorList->SetWidth("100%");
	ActorList->SetHeight("50%");
	ActorList->SetTop("0");
	ActorList->SetLeft("0");	
	Canvas->AddUnit(ActorList);

	ActorInfoList = NewUI<QFAUIList>();
	
	ActorTransform = NewUI<QFAUIActorTransform>();
	ActorInfoList->AddUnit(ActorTransform);
	ActorInfoList->SetEnable(false);
	
	ActorInfoList->SetWidth("100%");
	ActorInfoList->SetHeight("50%");
	ActorInfoList->SetTop("50%");
	ActorInfoList->SetLeft("0");
	Canvas->AddUnit(ActorInfoList);
}

QFAEditorGameViewportInfo::~QFAEditorGameViewportInfo()
{
	Canvas->Destroy();
	ActorList->Destroy();
	ActorTransform->Destroy();
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
	QFATextBackground* text = NewUI<QFATextBackground>();
	ActorList->AddUnit(text, actor);
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

QActor* QFAEditorGameViewportInfo::PressedDelete()
{
	QFAUIParent* unit = ActorList->GetSelectedUnit();
	if (!unit || !ActorList->GetFocus())
		return nullptr;

	for (size_t i = 0; i < ActorAndTextList.size(); i++)
	{
		if (ActorAndTextList[i].text == unit)
		{
			ActorList->RemoveUnit(ActorAndTextList[i].text);
			QActor* actor = ActorAndTextList[i].actor;
			ActorAndTextList[i].text->Destroy();
			ActorAndTextList.erase(ActorAndTextList.begin() + i);
			return actor;
		}
	}
}

void QFAEditorGameViewportInfo::RemoveAll()
{
	ActorList->RemoveAllUnit(true);
	ActorAndTextList.clear();
}



void QFAEditorGameViewportInfo::MySlotChange(QFAUIUnit* unit)
{
}

void QFAEditorGameViewportInfo::WidthChanged(int oldValue)
{
	((QFAEditorCanvas*)Canvas)->ParentSetWidth = Width;
	Canvas->SetWidth("100%", false);
}

void QFAEditorGameViewportInfo::HeightChanged(int oldValue)
{
	((QFAEditorCanvas*)Canvas)->ParentSetHeight = Height;
	Canvas->SetHeight("100%", false);
}

void QFAEditorGameViewportInfo::TopChanged(int oldValue)
{
	((QFAEditorCanvas*)Canvas)->ParentSetPosition_y = Position_y;
	Canvas->SetTop("0");
}

void QFAEditorGameViewportInfo::LeftChanged(int oldValue)
{
	((QFAEditorCanvas*)Canvas)->ParentSetPosition_x = Position_x;
	Canvas->SetLeft("0");
}

float QFAEditorGameViewportInfo::UpdateInnerHeight()
{
	return Height;
}

float QFAEditorGameViewportInfo::UpdateInnerWidth()
{
	return Width;
}
