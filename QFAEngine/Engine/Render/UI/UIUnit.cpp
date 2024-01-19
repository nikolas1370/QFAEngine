#include "UIUnit.h"
#include <Render/UI/UIParentComponent.h>

void QFAUIUnit::SetSlot(void* slot, int slotSize)
{
	if (slotSize > sizeof(SParentSlot))
		slotSize = sizeof(SParentSlot);

	memcpy(&Slot, slot, slotSize);

	Parent->MySlotChange(this);
}
