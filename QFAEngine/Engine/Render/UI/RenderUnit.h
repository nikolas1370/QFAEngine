#pragma once
#include <Render/UI/UIUnit.h>
#include <Tools/VulkanSuff.h>
class QFAWindow;
class QFAUIRenderUnit : public QFAUIUnit
{
	friend QFAWindow;

public:
	QFAUIRenderUnit();
	~QFAUIRenderUnit();

protected:
	virtual void Render(VkCommandBuffer comandebuffer) = 0;

};

