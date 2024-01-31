#pragma once
#include <Render/UI/UIUnit.h>
#include <Tools/VulkanSuff.h>
class QFAWindow;
class QFAVKPipeline;
class QFAUIRenderUnit : public QFAUIUnit
{
	friend QFAWindow;
	inline virtual QFAVKPipeline* GetPipeline() { return 0; }
public:
	QFAUIRenderUnit();
	~QFAUIRenderUnit();

	

protected:
	virtual void Render(VkCommandBuffer comandebuffer) = 0;		
};

