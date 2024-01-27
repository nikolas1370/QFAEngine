#include "UIUnit.h"
#include <Render/UI/UIParentComponent.h>
#include <Render/Window/Viewport.h>
void QFAUIUnit::SetPosition(unsigned int x, unsigned int y)
{
	if (!Parent || Parent->GetIsRoot())
		SetPositionParent(x, y);
}

void QFAUIUnit::SetSize(unsigned int w, unsigned int h)
{
	if (!Parent || Parent->GetIsRoot())
		SetSizeParent(w, h);
}

QFAUIUnit::~QFAUIUnit()
{
	if (Parent)
		Parent->RemoveUnitWithoutNotify(this);
}

void QFAUIUnit::SetSlot(void* slot)
{
	if (!slot)
		return;

	memcpy(&Slot, slot, ((QFAUISlot::SParentSlot*)slot)->structSize);
	if(Parent)
		Parent->MySlotChange(this);
}