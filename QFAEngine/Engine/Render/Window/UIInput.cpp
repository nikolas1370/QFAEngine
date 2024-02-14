#include "UIInput.h"
#include <Render/Window/QFAViewportRoot.h>

QFAUIInput::QFAUIInput()
{
}

QFAUIInput::~QFAUIInput()
{
}

void QFAUIInput::AddUnit(QFAUIUnit* unit)
{
	if (!unit)
		return;

	SortUIUnits.Add(unit);
	if (unit->CanBeParent)
	{
		QFAUIParentComponent* parent = (QFAUIParentComponent*)unit;

		for (size_t i = 0; i < parent->Children.Length(); i++)
			AddUnit(parent->Children[i]);
	}
}

void QFAUIInput::SortUIs(QFAViewportRoot* root)
{
	SortUIUnits.Clear();
	for (size_t i = 0; i < root->Children.Length(); i++)
		AddUnit(root->Children[i]);

	for (int i = SortUIUnits.Length() - 2; i > 0; i--)
	{
		for (size_t j = 0; j <= i; j++)
		{ // in start of array number bigger
			if (SortUIUnits[j + 1]->ZIndex > SortUIUnits[j]->ZIndex) // ZIndex forward == 1 
			{
				QFAUIUnit* pTem = SortUIUnits[j];
				SortUIUnits[j] = SortUIUnits[j + 1];
				SortUIUnits[j + 1] = pTem;
			}
		}
	}
}