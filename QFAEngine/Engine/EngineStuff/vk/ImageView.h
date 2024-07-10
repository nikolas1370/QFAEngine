#pragma once
#include <EngineStuff/VulkanSuff.h>
class QFAImage;
class QFAEXPORT QFAVKImageView
{
	/*
		Image objects are not directly accessed by pipeline shaders
		for reading or writing image data. Instead, image views representing
		contiguous ranges of the image subresources and containing additional
		metadata are used for that purpose. Views must be created on images of
		compatible types, and must represent a valid subset of image subresources.
	*/
	friend QFAImage;

public:
	VkImageView ImageView;

private:
	void CreateView(QFAImage* ti, VkImageAspectFlags as);
	void UpdateView(QFAVKImageView* view);
public:	
	QFAVKImageView();
	QFAVKImageView(QFAImage* ti, VkImageAspectFlags as = VK_IMAGE_ASPECT_COLOR_BIT);

	~QFAVKImageView();
};
