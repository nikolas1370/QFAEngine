#pragma once
#include <Tools/Array.h>

class QFAUIUnit;
class QFAViewportRoot;
class QFAUIInput
{
public:
	QFAUIInput();
	~QFAUIInput();


	

private:

	//in start of array number bigger
	QFAArray<QFAUIUnit*> SortUIUnits;
	void AddUnit(QFAUIUnit* unit);
	void SortUIs(QFAViewportRoot* root);
};

