#include "pch.h"
#include "UIUnit.h"
#include <UI/UIParentMultipleUnit.h>
#include <EngineStuff/Window/EngineViewport.h>
#include <EngineStuff/Window/UIEvent.h>
#include <Tools/String.h>

QFAUIUnit::~QFAUIUnit()
{
    if(IsValid())
	    if (Parent)
		    Parent->RemoveUnitWithoutNotify(this);

    UnitValid = false;
    QFAUIEvent::UnitUnderDelete(this);
}

bool QFAUIUnit::IsMyParent(QFAUIParent* parent)
{
    if (!parent || !Parent)
        return false;

    while (true)
    {
        if (Parent == parent)
            return true;

        parent = parent->GetParent();
        if (!parent)
            return false;
    }

    return false;
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

QFAEngineWindow* QFAUIUnit::GetWindow()
{
    unsigned int countUnit;
    QFAViewportRoot* vr = GetViewportRoot(countUnit);
    if (!vr)
        return nullptr;
    
    QFAEngineViewport* viewport = vr->GetViewport();
    if (!viewport)
        return nullptr;

    return viewport->GetWindow();
}

void QFAUIUnit::ProcessParentOverflow(UniformOverflow& param, QFAUIParent* parent)
{
    if (!parent)
        return;

    FVector2D parentPosition = FVector2D(parent->GetPosition());
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

   
        if (parent->Events.InFocus.fun)
            parent->Events.InFocus.fun(this, parent->Events.InFocus.userData);

        parent = parent->Parent;
    }
}

void QFAUIUnit::NotifyOutFocus(bool onlyOneUnit)
{
    if (onlyOneUnit)
    {
        if (this->Events.OutFocus.fun)
            this->Events.OutFocus.fun(this->Events.OutFocus.userData);
        return;
    }

    QFAUIUnit* parent = this;
    while (true)
    {
        if (!parent)
            return;

        if (parent->Events.OutFocus.fun)
            parent->Events.OutFocus.fun(parent->Events.OutFocus.userData);

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

        if (parent->Events.LeftMouseDown.fun)
            parent->Events.LeftMouseDown.fun(this, parent->Events.LeftMouseDown.userData);

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

        if (parent->Events.LeftMouseUp.fun)
            parent->Events.LeftMouseUp.fun(this, parent->Events.LeftMouseUp.userData);

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

        if (parent->Events.RightMouseDown.fun)
            parent->Events.RightMouseDown.fun(this, parent->Events.RightMouseDown.userData);

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

        if (parent->Events.RightMouseUp.fun)
            parent->Events.RightMouseUp.fun(this, parent->Events.RightMouseUp.userData);

        parent = parent->Parent;
    }
}

void QFAUIUnit::NotifyForwardMouseDown()
{
    QFAUIUnit* parent = this;
    while (true)
    {
        if (!parent)
            return;

        if (parent->Events.ForwardMouseDown.fun)
            parent->Events.ForwardMouseDown.fun(this, parent->Events.ForwardMouseDown.userData);

        parent = parent->Parent;
    }
}

void QFAUIUnit::NotifyBackwardMouseDown()
{
    QFAUIUnit* parent = this;
    while (true)
    {
        if (!parent)
            return;

        if (parent->Events.BackwardMouseDown.fun)
            parent->Events.BackwardMouseDown.fun(this, parent->Events.BackwardMouseDown.userData);

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

        if (parent->Events.LeftMouseDownUp.fun)
            parent->Events.LeftMouseDownUp.fun(this, parent->Events.LeftMouseDownUp.userData);

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

        if (parent->Events.RightMouseDownUp.fun)
            parent->Events.RightMouseDownUp.fun(this, parent->Events.RightMouseDownUp.userData);

        parent = parent->Parent;
    }
}

void QFAUIUnit::SetWidth(const char* width, bool parentSetWidthMinus)
{
    StrWidth = width;
    ParentSetWidthMinus = parentSetWidthMinus;
    int parentW;
    if (GetIsRoot())
    {
        QFAViewportRoot* root = (QFAViewportRoot*)this;
        if (!root->GetViewport())
            return;

        parentW = root->GetViewport()->GetSize().X;
    }
    else
    {
        if (!Parent)
            return;

        parentW = Parent->GetSize().X;
    }   

    int old = Width;
    if (!StrWidth)
    {        
        Width = ParentSetWidth;
        if (CanBeParent)
            ((QFAUIParent*)this)->RecalculateBackgroundSize();

        WidthChanged(old);
        return;
    }

    if (parentSetWidthMinus)
        Width = ParentSetWidth - QFAString::GetValue(StrWidth, parentW, false);
    else
        Width = QFAString::GetValue(StrWidth, parentW, false);

    if (CanBeParent)
        ((QFAUIParent*)this)->RecalculateBackgroundSize();

    WidthChanged(old);
}

void QFAUIUnit::SetHeight(const char* height, bool parentSetHeightMinus)
{
    StrHeight = height;
    ParentSetHeightMinus = parentSetHeightMinus;
    int parentH;
    if (GetIsRoot())
    {
        QFAViewportRoot* root = (QFAViewportRoot*)this;
        if (!root->GetViewport())
            return;

        parentH = root->GetViewport()->GetSize().Y;
    }
    else
    {
        if (!Parent)
            return;

        parentH = Parent->GetSize().Y;
    }

    int old = Height;
    if (!StrHeight)
    {
        Height = ParentSetHeight;
        if (CanBeParent)
            ((QFAUIParent*)this)->RecalculateBackgroundSize();

        HeightChanged(old);
        return;
    }

    if (parentSetHeightMinus)
        Height = ParentSetHeight - QFAString::GetValue(StrHeight, parentH, false);
    else
        Height = QFAString::GetValue(StrHeight, parentH, false);

    if (CanBeParent)
        ((QFAUIParent*)this)->RecalculateBackgroundSize();

    HeightChanged(old);
}

void QFAUIUnit::SetTop(const char* top)
{
    StrTop = top;
    int parentT;
    if (GetIsRoot())
    {
        QFAViewportRoot* root = (QFAViewportRoot*)this;
        if (!root->GetViewport())
            return;

        parentT = root->GetViewport()->GetSize().Y;
    }
    else
    {
        if (!Parent)
            return;

        parentT = Parent->GetSize().Y;
    }

    int old = Position_y;
    if (StrTop)
        Position_y = ParentSetPosition_y + QFAString::GetValue(StrTop, parentT, false);
    else
        Position_y = ParentSetPosition_y;

    if (CanBeParent)
        ((QFAUIParent*)this)->RecalculateBackgroundPosition();

    TopChanged(old);
}

void QFAUIUnit::SetLeft(const char* left)
{
    StrLeft = left;
    int parentL;
    if (GetIsRoot())
    {
        QFAViewportRoot* root = (QFAViewportRoot*)this;
        if (!root->GetViewport())
            return;

        parentL = root->GetViewport()->GetSize().X;
    }
    else
    {
        if (!Parent)
            return;

        parentL = Parent->GetSize().X;
    }

    int old = Position_x;
    if (StrLeft)
        Position_x = ParentSetPosition_x + QFAString::GetValue(StrLeft, parentL, false);
    else
        Position_x = ParentSetPosition_x;

    if (CanBeParent)
        ((QFAUIParent*)this)->RecalculateBackgroundPosition();

    LeftChanged(old);
}


/*-------- EventFunctions --------*/
void QFAUIUnit::EventFunctions::SetInFocus(void (*fun)(QFAUIUnit*, void* ), void* userData)
{
    InFocus.fun = fun;
    InFocus.userData = userData;
 //   InFocus = fun;
}

void QFAUIUnit::EventFunctions::SetOutFocus(void (*fun)(void*), void* userData)
{
    OutFocus.fun = fun;
    OutFocus.userData = userData;
}

void QFAUIUnit::EventFunctions::SetLeftMouseDown(void (*fun)(QFAUIUnit*, void*), void* userData)
{
    LeftMouseDown.fun = fun;
    LeftMouseDown.userData = userData;
}

void QFAUIUnit::EventFunctions::SetLeftMouseUp(void (*fun)(QFAUIUnit*, void*), void* userData)
{
    LeftMouseUp.fun = fun;
    LeftMouseUp.userData = userData;
}

void QFAUIUnit::EventFunctions::SetRightMouseDown(void (*fun)(QFAUIUnit*, void*), void* userData)
{
    RightMouseDown.fun = fun;
    RightMouseDown.userData = userData;
}

void QFAUIUnit::EventFunctions::SetRightMouseUp(void (*fun)(QFAUIUnit*, void*), void* userData)
{
    RightMouseUp.fun = fun;
    RightMouseUp.userData = userData;
}

void QFAUIUnit::EventFunctions::SetForwardMouseDown(void(*fun)(QFAUIUnit*, void*), void* userData)
{
    ForwardMouseDown.fun = fun;
    ForwardMouseDown.userData = userData;
}

void QFAUIUnit::EventFunctions::SetBackwardMouseDown(void(*fun)(QFAUIUnit*, void*), void* userData)
{
    BackwardMouseDown.fun = fun;
    BackwardMouseDown.userData = userData;
}

void QFAUIUnit::EventFunctions::SetLeftMouseDownUp(void (*fun)(QFAUIUnit*, void*), void* userData)
{
    LeftMouseDownUp.fun = fun;
    LeftMouseDownUp.userData = userData;
}

void QFAUIUnit::EventFunctions::SetRightMouseDownUp(void (*fun)(QFAUIUnit*, void*), void* userData)
{
    RightMouseDownUp.fun = fun;
    RightMouseDownUp.userData = userData;
}
