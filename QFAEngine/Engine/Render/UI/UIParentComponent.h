#pragma once
#include <Render/UI/UIUnit.h>
#include <Tools/Array.h>
class QFAViewport;
class QFAWindow;
class QFAUIUnit;

class QFAUIParentComponent : public QFAUIUnit
{
	friend QFAUIUnit;
	friend QFAViewport;
	friend QFAWindow;	
protected:
	QFAArray<QFAUIUnit*> Children;
	// call if AddUnit success
	virtual void NewUnit(QFAUIUnit* unit) = 0;
	// child call if slot change
	virtual void MySlotChange(QFAUIUnit* unit) = 0;
public:

	QFAUIParentComponent();

	void AddUnit(QFAUIUnit* unit);
	void removeUnit(QFAUIUnit* unit);
};