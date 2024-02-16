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

   
        if (parent->Events.InFocus)
            parent->Events.InFocus(this);

        parent = parent->Parent;
    }
}

void QFAUIUnit::NotifyOutFocus(bool onlyOneUnit)
{
    if (onlyOneUnit)
    {
        if (this->Events.InFocus)
            this->Events.OutFocus();
        return;
    }

    QFAUIUnit* parent = this;
    while (true)
    {
        if (!parent)
            return;

        if (parent->Events.InFocus)
            parent->Events.OutFocus();

        parent = parent->Parent;
    }
}

void QFAUIUnit::NotifyLeftMouseDown()
{
    QFAUIUnit* parent = this;
    while (true)
    {
        if (!parent)
            return;

        if (parent->Events.LeftMouseDown)
            parent->Events.LeftMouseDown(this);

        parent = parent->Parent;
    }
}

void QFAUIUnit::NotifyLeftMouseUp()
{
    QFAUIUnit* parent = this;
    while (true)
    {
        if (!parent)
            return;

        if (parent->Events.LeftMouseUp)
            parent->Events.LeftMouseUp(this);

        parent = parent->Parent;
    }
}

void QFAUIUnit::NotifyRightMouseDown()
{
    QFAUIUnit* parent = this;
    while (true)
    {
        if (!parent)
            return;

        if (parent->Events.RightMouseDown)
            parent->Events.RightMouseDown(this);

        parent = parent->Parent;
    }
}

void QFAUIUnit::NotifyRightMouseUp()
{
    QFAUIUnit* parent = this;
    while (true)
    {
        if (!parent)
            return;

        if (parent->Events.RightMouseUp)
            parent->Events.RightMouseUp(this);

        parent = parent->Parent;
    }
}

void QFAUIUnit::NotifyLeftMouseDownUp()
{
    QFAUIUnit* parent = this;
    while (true)
    {
        if (!parent)
            return;

        if (parent->Events.LeftMouseDownUp)
            parent->Events.LeftMouseDownUp(this);

        parent = parent->Parent;
    }
}

void QFAUIUnit::NotifyRightMouseDownUp()
{
    QFAUIUnit* parent = this;
    while (true)
    {
        if (!parent)
            return;

        if (parent->Events.RightMouseDownUp)
            parent->Events.RightMouseDownUp(this);

        parent = parent->Parent;
    }
}


/*-------- EventFunctions --------*/
void QFAUIUnit::EventFunctions::SetInFocus(std::function<void(QFAUIUnit*)> fun)
{
    InFocus = fun;
}

void QFAUIUnit::EventFunctions::SetOutFocus(std::function<void()> fun)
{
    OutFocus = fun;
}

void QFAUIUnit::EventFunctions::SetLeftMouseDown(std::function<void(QFAUIUnit*)> fun)
{
    LeftMouseDown = fun;
}

void QFAUIUnit::EventFunctions::SetLeftMouseUp(std::function<void(QFAUIUnit*)> fun)
{
    LeftMouseUp = fun;
}

void QFAUIUnit::EventFunctions::SetRightMouseDown(std::function<void(QFAUIUnit*)> fun)
{
    RightMouseDown = fun;
}

void QFAUIUnit::EventFunctions::SetRightMouseUp(std::function<void(QFAUIUnit*)> fun)
{
    RightMouseUp = fun;
}

void QFAUIUnit::EventFunctions::SetLeftMouseDownUp(std::function<void(QFAUIUnit*)> fun)
{
    LeftMouseDownUp = fun;
}

void QFAUIUnit::EventFunctions::SetRightMouseDownUp(std::function<void(QFAUIUnit*)> fun)
{
    RightMouseDownUp = fun;
}