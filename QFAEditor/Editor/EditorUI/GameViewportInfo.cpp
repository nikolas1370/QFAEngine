#include "GameViewportInfo.h"
#include <Render/UI/UIList.h>
#include <Render/UI/Scroll.h>

#include <Object/Actor/Actor.h>
#include <Render/UI/Canvas.h>

#include <EditorUI/UIActorList.h>

QFAEditorGameViewportInfo::QFAEditorGameViewportInfo()
{	
	ActorList = new QFAEditorUIActorList;
	AddHiddenChild(ActorList);

}

QFAEditorGameViewportInfo::~QFAEditorGameViewportInfo()
{
	delete ActorList;

}



void QFAEditorGameViewportInfo::MySlotChange(QFAUIUnit* unit)
{
}

void QFAEditorGameViewportInfo::ChangeSize(unsigned int w, unsigned int h)
{
	Width = w;
	Height = h;
	SetChildSize(Children[0], w, h);	
}

void QFAEditorGameViewportInfo::ChangePosition(int x, int y)
{
	Position_x = x;
	Position_y = y;
	SetChildPosition(Children[0], Position_x, Position_y);
}

float QFAEditorGameViewportInfo::UpdateInnerHeight()
{
	return Height;
}

float QFAEditorGameViewportInfo::UpdateInnerWidth()
{
	return Width;
}
