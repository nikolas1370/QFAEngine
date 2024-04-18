#include "GameViewportInfo.h"
#include <Render/UI/UIList.h>
#include <Render/UI/Scroll.h>

#include <Object/Actor/Actor.h>
#include <Render/UI/Canvas.h>

#include <EditorUI/UIActorList.h>
#include <EditorUI/UIActorTransform.h>




QFAEditorGameViewportInfo::QFAEditorGameViewportInfo()
{	
	Canvas = new QFAUICanvas;
	AddHiddenChild(Canvas);

	QFAUISlot::SCanvasSlot slot;

	ActorList = new QFAEditorUIActorList([this](QActor* actor) 
		{
			this->SelectActor(actor); 
		});

	slot.x = 0.0f;
	slot.y = 0.0f;
	slot.Width = 1.0f;
	slot.Height = 0.5f;	
	ActorList->SetSlot(&slot);
	Canvas->AddUnit(ActorList);


	ActorInfoSCroll = new QFAUIScroll;
	Canvas->AddUnit(ActorInfoSCroll);
	slot.y = 0.5f;
	ActorInfoSCroll->SetSlot(&slot);
	
	ActorInfoList = new QFAUIList;
	ActorInfoSCroll->SetUnit(ActorInfoList);
	
	ActorTransform = new QFAUIActorTransform;
	ActorInfoList->AddUnit(ActorTransform);
	ActorInfoList->SetEnable(false);
}

QFAEditorGameViewportInfo::~QFAEditorGameViewportInfo()
{
	delete Canvas;
	delete ActorList;
	delete ActorTransform;
}

void QFAEditorGameViewportInfo::SelectActor(QActor* actor)
{
	ActorInfoList->SetEnable(actor->IsValid());

	ActorList->SelectActor(actor);
	ActorTransform->SelectActor(actor);
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
