#pragma once
#include <Render/UI/UIUnit.h>
#include <Tools/VulkanSuff.h>
class QFAWindow;
class QFAVKPipeline;
class QFAUIScroll;
class QFAUIParentOneUnit;
class QFAUIParentMultipleUnit;
class QFAEXPORT QFAUIRenderUnit : public QFAUIUnit
{
	friend QFAWindow;
	friend QFAUIScroll;
	friend QFAUIParentOneUnit;
	friend QFAUIParentMultipleUnit;
	inline virtual QFAVKPipeline* GetPipeline() { return 0; }
public:
	QFAUIRenderUnit();
	~QFAUIRenderUnit();

	

protected:
	virtual void Render(VkCommandBuffer comandebuffer) = 0;		


	/*
		write to shader
		only one closest parent can write
	*/
	float UnitScroll = 0.0f; // offset y
	
	/*
		only one closest parent can write
		onli for text
	*/
	float UnitOffsetX = 0.0f;

};

