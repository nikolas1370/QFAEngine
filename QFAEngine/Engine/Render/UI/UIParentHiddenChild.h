#pragma once
#include <Render/UI/UIParent.h>

/*
	QFAParentHiddenChild does not have slot because child Hidden

	only child class can add child unit in Children
*/
class QFAWindow;
class QFAUIEvent;
class QFAParentHiddenChild : public QFAUIParent
{
	friend QFAWindow;
	friend QFAUIEvent;
public:
	QFAParentHiddenChild();
	~QFAParentHiddenChild();

protected:
	std::vector<QFAUIUnit*> Children;

	void GetChildren(std::vector<QFAUIUnit*>& children);

	// only child class can use it
	void AddHiddenChild(QFAUIUnit* unit);
private:
	void RemoveUnitWithoutNotify(QFAUIUnit* unit) final {};

	void ParentEnable() override {};

	void ParentDisable() override {};

	void ParentAttach() override {};

	void ParentDisconect() override {} ;

};

