#include "pch.h"
#include "UIParentHiddenChild.h"

QFAParentHiddenChild::QFAParentHiddenChild()
{
	ParentType = EParentType::HiddenChild;

}

QFAParentHiddenChild::~QFAParentHiddenChild()
{
}

void QFAParentHiddenChild::GetChildren(std::vector<QFAUIUnit*>& children)
{
	for (size_t i = 0; i < Children.size(); i++)
		children.push_back(Children[i]);
}

void QFAParentHiddenChild::AddHiddenChild(QFAUIUnit* unit)
{
	unit->Parent = (QFAUIParent*)this;
	Children.push_back(unit);
}