#pragma once
#include <UI/UIParent.h>

/*
	QFAParentHiddenChild does not have slot because child Hidden

	only child class can add child unit in Children
*/
class QFAEngineWindow;
class QFAUIEvent;
class QFAEXPORT QFAParentHiddenChild : public QFAUIParent
{
	friend QFAEngineWindow;
	friend QFAUIEvent;

protected:
	// not add child directly use AddHiddenChild
	std::vector<QFAUIUnit*> Children;

private:
	void RemoveUnitWithoutNotify(QFAUIUnit* unit) final {};

	void ParentEnable() override {};

	void ParentDisable() override {};

	void ParentAttach() override {};

	void ParentDisconect() override {};

protected:
	void GetChildren(std::vector<QFAUIUnit*>& children);

	/*
		 only child class can use it

		 this function not set child position/size

	*/
	void AddHiddenChild(QFAUIUnit* unit);

public:
	QFAParentHiddenChild();
	~QFAParentHiddenChild();

};

