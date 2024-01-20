#include "UIParentComponent.h"
#include <Render/Window/Viewport.h>

QFAUIParentComponent::QFAUIParentComponent()
{
	CanBeParent = true;
}

void QFAUIParentComponent::AddUnit(QFAUIUnit* unit)
{
	if (!unit || unit == this)// check if unit is parent 
		return;

	if (unit->Parent)
		unit->Parent->removeUnit(unit);
	else if (unit->ParentViewport)
		unit->ParentViewport->RemoveUnit(unit);

	Children.Add(unit);
	unit->Parent = this;
	NewUnit(unit);
}

void QFAUIParentComponent::removeUnit(QFAUIUnit* unit)
{
	if (!unit)
		return;

	Children.Remove(unit);
	unit->Parent = nullptr;
}
