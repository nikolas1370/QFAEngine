#pragma once
#include <Tools/Debug/VulkanSuff.h>
class QFAVKTextureSampler
{
	/*
	*	not create for each texture only for different setting( texture setting )
	* 
		objects represent the state of an image sampler which
		is used by the implementation to read image data and
		apply filtering and other transformations for the shader.

		Note the sampler does not reference a VkImage anywhere. The sampler is a
		distinct object that provides an interface to extract colors from a texture.
		It can be applied to any image you want, whether it is 1D, 2D or 3D. This
		is different from many older APIs, which combined texture images and
		filtering into a single state.
	*/
	
public:
	VkSampler textureSampler;
	QFAVKTextureSampler();
	~QFAVKTextureSampler();

private:

};
