#include "UIUnit.h"
#include <Render/UI/UIParentMultipleUnit.h>
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
    if(IsValid())
	    if (Parent)
		    Parent->RemoveUnitWithoutNotify(this);

    UnitValid = false;
}

void QFAUIUnit::SetSlot(void* slot)
{
	if (!slot)
		return;

	memcpy(&Slot, slot, ((QFAUISlot::SParentSlot*)slot)->BaseInfo.structSize);
	if(Parent)
		Parent->MySlotChange(this);
}

QFAViewportRoot* QFAUIUnit::GetViewportRoot(unsigned int& countUnit)
{
    countUnit = 0;

    QFAUIUnit* parent = this;
    while (true)
    {        
        if (!parent)
            return nullptr;
        
        countUnit++;
        if (parent->Type == QFAUIType::Type::ViewportRoot)
            return (QFAViewportRoot*)parent;
        else
            parent = parent->Parent;
    }

    return nullptr;
}

void QFAUIUnit::EventFunctions::SetInFocusFunction(std::function<void(QFAUIUnit*)> fun)
{
    FunInFocus = fun;
}

void QFAUIUnit::EventFunctions::SetOutFocusFunction(std::function<void()> fun)
{
    FunOutFocus = fun;
}

void QFAUIUnit::ProcessParentOverflow(UniformOverflow& param, QFAUIParent* parent)
{
    if (!parent)
        return;

    FVector2D parentPosition = parent->GetPosition();
    FVector2D parentSize = parent->GetSize();

    if (parent->GetOverflow() == QFAUIParent::Hidden)
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
    else if (parent->GetOverflow() == QFAUIParent::HiddenHorizon)
    {
        param.enable = 1;
        if (parentPosition.X > param.leftTopX)
            param.leftTopX = parentPosition.X;

        float tem = parentPosition.X + parentSize.X;
        if (tem < param.rightBottomX)
            param.rightBottomX = tem;
    }
    else if (parent->GetOverflow() == QFAUIParent::HiddenVertical)
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

float QFAUIUnit::ProcessParentOpacity(float childOpacity, QFAUIParent* parent)
{
    if (!parent)
        return childOpacity;

    return ProcessParentOpacity(childOpacity * parent->Opacity, parent->GetParent());
}

void QFAUIUnit::NotifyInFocus()
{
    QFAUIUnit* parent = this;
    while (true)
    {
        if (!parent)
            return;

   
        if (parent->Events.FunInFocus)
            parent->Events.FunInFocus(this);

        parent = parent->Parent;
    }
}

void QFAUIUnit::NotifyOutFocus(bool onlyOneUnit)
{
    if (onlyOneUnit)
    {
        if (this->Events.FunInFocus)
            this->Events.FunOutFocus();
        return;
    }

    QFAUIUnit* parent = this;
    while (true)
    {
        if (!parent)
            return;

        if (parent->Events.FunInFocus)
            parent->Events.FunOutFocus();

        parent = parent->Parent;
    }
}