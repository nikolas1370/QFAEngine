#include "UIUnit.h"
#include <Render/UI/UIParentComponent.h>

void QFAUIUnit::SetSlot(void* slot)
{
	if (!slot)
		return;

	memcpy(&Slot, slot, ((QFAUISlot::SParentSlot*)slot)->structSize);
	Parent->MySlotChange(this);
}