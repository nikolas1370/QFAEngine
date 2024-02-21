#pragma once
#include <Render/UI/UIParent.h>

/*
	child of this class have only one UI child
	QFAParentHiddenChild does not have slot because child Hidden
*/
class QFAWindow;
class QFAParentHiddenChild : public QFAUIParent
{
	friend QFAWindow;
public:
	QFAParentHiddenChild();
	~QFAParentHiddenChild();

protected:
	virtual QFAVKPipeline* GetChildPipeline() = 0;
	virtual void RenderChild(VkCommandBuffer comandebuffer) = 0;	
	virtual QFAUIUnit* GetChild() = 0;
private:
	void RemoveUnitWithoutNotify(QFAUIUnit* unit) final {};

	void ParentEnable() override {};

	void ParentDisable() override {};

	void ParentAttach() override {};

	void ParentDisconect() override {} ;

};

