#include "UIUnit.h"
#include <Render/UI/UIParentComponent.h>
#include <Render/Window/Viewport.h>
QFAUIUnit::~QFAUIUnit()
{
	if (Parent)
		Parent->RemoveUnitWithoutNotify(this);
	else if (ParentViewport)
		ParentViewport->RemoveUnitWithoutNotify(this);
}

void QFAUIUnit::SetSlot(void* slot)
{
	if (!slot)
		return;

	memcpy(&Slot, slot, ((QFAUISlot::SParentSlot*)slot)->structSize);
	Parent->MySlotChange(this);
}