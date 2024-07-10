#pragma once
#include <UI/UIParent.h>

class QFAEngineWindow;
class QFAUIEvent;
class QFAEXPORT QFAUIParentOneUnit : public QFAUIParent
{
	friend QFAEngineWindow;
	friend QFAUIEvent;

protected:
	QFAUIUnit* Child = nullptr;
	// call if SetUnit success
	virtual void ChangedUnit(QFAUIUnit* unit) = 0;

	void RemoveUnitWithoutNotify(QFAUIUnit* unit) final;
	void ParentEnable() final;
	void ParentDisable() final;
	void ParentAttach() final;
	void ParentDisconect() final;

public:
	QFAUIParentOneUnit();
	~QFAUIParentOneUnit();

	void SetUnit(QFAUIUnit* unit);
};