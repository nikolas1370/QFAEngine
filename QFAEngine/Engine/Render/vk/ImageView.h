#pragma once
#include <Tools/VulkanSuff.h>
class QFAImage;
class QFAVKImageView
{
	/*
		Image objects are not directly accessed by pipeline shaders
		for reading or writing image data. Instead, image views representing
		contiguous ranges of the image subresources and containing additional
		metadata are used for that purpose. Views must be created on images of
		compatible types, and must represent a valid subset of image subresources.
	*/
	
public:
	VkImageView ImageView;
	QFAVKImageView(QFAImage* ti, VkImageAspectFlags as = VK_IMAGE_ASPECT_COLOR_BIT);

	~QFAVKImageView();
	
private:

};
