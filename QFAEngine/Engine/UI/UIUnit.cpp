#include "pch.h"
#include "UIUnit.h"
#include <UI/UIParentMultipleUnit.h>
#include <Core/EngineStuff/Window/EngineViewport.h>
#include <Core/EngineStuff/Window/UIEvent.h>
#include <Tools/String.h>
#include <UI/UIParent.h>
std::vector<QFAUIUnit*> QFAUIUnit::ListNeedFree;
std::vector<std::function<void(QFAUIUnit* unit)>> QFAUIUnit::EventWithParam;
std::vector<std::function<void()>> QFAUIUnit::EventWithOutParam;

QFAUIUnit::~QFAUIUnit()
{
    if(IsValid())
	    if (Parent)
		    Parent->RemoveUnitWithoutNotify(this);

    QFAUIEvent::UnitUnderDelete(this);

    if (Events.Funs)
        delete Events.Funs;

    if (Parent)
        Parent->ChildUnderDelete(this);
}

void QFAUIUnit::Destroy()
{
    if (IsValid())
    {
        if (CreateInHeap)
            ListNeedFree.push_back(this);

        this->~QFAUIUnit();
        NeedFree = true;
    }
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

void QFAUIUnit::Init()
{
    ListNeedFree.reserve(100);
}

void QFAUIUnit::FreeUnits()
{
    for (size_t i = 0; i < ListNeedFree.size(); i++)
        free(ListNeedFree[i]);

    ListNeedFree.clear();
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
    EventWithParam.clear();
    QFAUIUnit* parent = this;
    while (true)
    {
        if (!parent)
            break;

        if (parent->Events.Funs && parent->Events.Funs->InFocus)
            EventWithParam.push_back(parent->Events.Funs->InFocus);

        parent = parent->Parent;
    }

    for (size_t i = 0; i < EventWithParam.size(); i++)
        EventWithParam[i](this);
}

void QFAUIUnit::NotifyOutFocus(bool onlyOneUnit)
{
    EventWithOutParam.clear();
    if (onlyOneUnit)
    {
        if (this->Events.Funs &&  this->Events.Funs->OutFocus)
            this->Events.Funs->OutFocus();

        return;
    }

    QFAUIUnit* parent = this;
    while (true)
    {
        if (!parent)
            break;

        if (parent->Events.Funs && parent->Events.Funs->OutFocus)
            EventWithOutParam.push_back(parent->Events.Funs->OutFocus);

        parent = parent->Parent;
    }

    for (size_t i = 0; i < EventWithOutParam.size(); i++)
        EventWithOutParam[i]();
}

void QFAUIUnit::NotifyLeftMouseDown()
{
    EventWithParam.clear();
    QFAUIUnit* parent = this;
    while (true)
    {
        if (!parent)
            break;

        if (parent->Events.Funs && parent->Events.Funs->LeftMouseDown)
            EventWithParam.push_back(parent->Events.Funs->LeftMouseDown);

        parent = parent->Parent;
    }

    for (size_t i = 0; i < EventWithParam.size(); i++)
        EventWithParam[i](this);
}

void QFAUIUnit::NotifyLeftMouseUp()
{
    EventWithParam.clear();
    QFAUIUnit* parent = this;
    while (true)
    {
        if (!parent)
            break;

        if (parent->Events.Funs && parent->Events.Funs->LeftMouseUp)
            EventWithParam.push_back(parent->Events.Funs->LeftMouseUp);

        parent = parent->Parent;
    }

    for (size_t i = 0; i < EventWithParam.size(); i++)
        EventWithParam[i](this);
}

void QFAUIUnit::NotifyRightMouseDown()
{
    EventWithParam.clear();
    QFAUIUnit* parent = this;
    while (true)
    {
        if (!parent)
            break;

        if (parent->Events.Funs && parent->Events.Funs->RightMouseDown)
            EventWithParam.push_back(parent->Events.Funs->RightMouseDown);

        parent = parent->Parent;
    }

    for (size_t i = 0; i < EventWithParam.size(); i++)
        EventWithParam[i](this);
}

void QFAUIUnit::NotifyRightMouseUp()
{
    EventWithParam.clear();
    QFAUIUnit* parent = this;
    while (true)
    {
        if (!parent)
            break;

        if (parent->Events.Funs && parent->Events.Funs->RightMouseUp)
            EventWithParam.push_back(parent->Events.Funs->RightMouseUp);

        parent = parent->Parent;
    }

    for (size_t i = 0; i < EventWithParam.size(); i++)
        EventWithParam[i](this);
}

void QFAUIUnit::NotifyForwardMouseDown()
{
    EventWithParam.clear();
    QFAUIUnit* parent = this;
    while (true)
    {
        if (!parent)
            break;

        if (parent->Events.Funs && parent->Events.Funs->ForwardMouseDown)
            EventWithParam.push_back(parent->Events.Funs->ForwardMouseDown);

        parent = parent->Parent;
    }

    for (size_t i = 0; i < EventWithParam.size(); i++)
        EventWithParam[i](this);
}

void QFAUIUnit::NotifyBackwardMouseDown()
{
    EventWithParam.clear();
    QFAUIUnit* parent = this;
    while (true)
    {
        if (!parent)
            break;

        if (parent->Events.Funs && parent->Events.Funs->BackwardMouseDown)
            EventWithParam.push_back(parent->Events.Funs->BackwardMouseDown);

        parent = parent->Parent;
    }

    for (size_t i = 0; i < EventWithParam.size(); i++)
        EventWithParam[i](this);
}

void QFAUIUnit::NotifyLeftMouseDownUp()
{
    EventWithParam.clear();
    QFAUIUnit* parent = this;
    while (true)
    {
        if (!parent)
            break;

        if (parent->Events.Funs && parent->Events.Funs->LeftMouseDownUp)
            EventWithParam.push_back(parent->Events.Funs->LeftMouseDownUp);

        parent = parent->Parent;
    }

    for (size_t i = 0; i < EventWithParam.size(); i++)
        EventWithParam[i](this);
}

void QFAUIUnit::NotifyRightMouseDownUp()
{
    EventWithParam.clear(); 
    QFAUIUnit* parent = this;
    while (true)
    {
        if (!parent)
            break;

        if (parent->Events.Funs && parent->Events.Funs->RightMouseDownUp)
            EventWithParam.push_back(parent->Events.Funs->RightMouseDownUp);

        parent = parent->Parent;
    }

    for (size_t i = 0; i < EventWithParam.size(); i++)
        EventWithParam[i](this);
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
void QFAUIUnit::UnitEvents::SetInFocus(std::function<void(QFAUIUnit* unit)> fun)
{
    if (!Funs)
        Funs = new EventFunctions;

    Funs->InFocus = fun;
}

void QFAUIUnit::UnitEvents::SetOutFocus(std::function<void()> fun)
{
    if (!Funs)
        Funs = new EventFunctions;

    Funs->OutFocus = fun;
}

void QFAUIUnit::UnitEvents::SetLeftMouseDown(std::function<void(QFAUIUnit* unit)> fun)
{
    if (!Funs)
        Funs = new EventFunctions;

    Funs->LeftMouseDown = fun;
}

void QFAUIUnit::UnitEvents::SetLeftMouseUp(std::function<void(QFAUIUnit* unit)> fun)
{
    if (!Funs)
        Funs = new EventFunctions;

    Funs->LeftMouseUp = fun;
}

void QFAUIUnit::UnitEvents::SetRightMouseDown(std::function<void(QFAUIUnit* unit)> fun)
{
    if (!Funs)
        Funs = new EventFunctions;

    Funs->RightMouseDown = fun;
}

void QFAUIUnit::UnitEvents::SetRightMouseUp(std::function<void(QFAUIUnit* unit)> fun)
{
    if (!Funs)
        Funs = new EventFunctions;

    Funs->RightMouseUp = fun;
}

void QFAUIUnit::UnitEvents::SetForwardMouseDown(std::function<void(QFAUIUnit* unit)> fun)
{
    if (!Funs)
        Funs = new EventFunctions;

    Funs->ForwardMouseDown = fun;
}

void QFAUIUnit::UnitEvents::SetBackwardMouseDown(std::function<void(QFAUIUnit* unit)> fun)
{
    if (!Funs)
        Funs = new EventFunctions;

    Funs->BackwardMouseDown = fun;
}

void QFAUIUnit::UnitEvents::SetLeftMouseDownUp(std::function<void(QFAUIUnit* unit)> fun)
{
    if (!Funs)
        Funs = new EventFunctions;

    Funs->LeftMouseDownUp = fun;
}

void QFAUIUnit::UnitEvents::SetRightMouseDownUp(std::function<void(QFAUIUnit* unit)> fun)
{
    if (!Funs)
        Funs = new EventFunctions;

    Funs->RightMouseDownUp = fun;
}
