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

	memcpy(&Slot, slot, ((QFAUISlot::SParentSlot*)slot)->BaseInfo.structSize);
	if(Parent)
		Parent->MySlotChange(this);
}

void QFAUIUnit::ProcessParentOverflow(UniformOverflow& param, QFAUIParentComponent* parent)
{
    if (!parent)
        return;

    FVector2D parentPosition = parent->GetPosition();
    FVector2D parentSize = parent->GetSize();

    if (parent->GetOverflow() == QFAUIParentComponent::Hidden)
    {
        param.enable = 1;
        if (parentPosition.X > param.leftTopX)
            param.leftTopX = parentPosition.X;

        if (parentPosition.Y > param.leftTopY)
            param.leftTopY = parentPosition.Y;

        float tem = parentPosition.X + parentSize.X;
        if (tem < param.rightBottomX)
            param.rightBottomX = tem;

        tem = parentPosition.Y + parentSize.Y;
        if (tem < param.rightBottomY)
            param.rightBottomY = parentPosition.Y + parentSize.Y;
    }
    else if (parent->GetOverflow() == QFAUIParentComponent::HiddenHorizon)
    {
        param.enable = 1;
        if (parentPosition.X > param.leftTopX)
            param.leftTopX = parentPosition.X;

        float tem = parentPosition.X + parentSize.X;
        if (tem < param.rightBottomX)
            param.rightBottomX = tem;
    }
    else if (parent->GetOverflow() == QFAUIParentComponent::HiddenVertical)
    {
        param.enable = 1;
        if (parentPosition.Y > param.leftTopY)
            param.leftTopY = parentPosition.Y;

        float tem = parentPosition.Y + parentSize.Y;
        if (tem < param.rightBottomY)
            param.rightBottomY = parentPosition.Y + parentSize.Y;
    }

    ProcessParentOverflow(param, parent->GetParent());
}

float QFAUIUnit::ProcessParentOpacity(float childOpacity, QFAUIParentComponent* parent)
{
    if (!parent)
        return childOpacity;

    return ProcessParentOpacity(childOpacity * parent->Opacity, parent->GetParent());
}