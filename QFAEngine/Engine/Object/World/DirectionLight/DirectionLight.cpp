#include "DirectionLight.h"
#include <Tools/Debug/VulkanSuff.h>


QDirectionLight::QDirectionLight()
{

}

QDirectionLight::~QDirectionLight()
{
}




glm::mat4 QDirectionLight::GetLightMatrix(const FVector& cameraPositionVulkan)
{
	FVector center = cameraPositionVulkan + Direction;

	float near_plane = 0.1f, far_plane = 100000;
	glm::mat4 depthProjectionMatrix = glm::orthoLH_ZO(-1000.0f, 1000.0f, -1000.0f, 1000.0f, near_plane, far_plane);
	glm::mat4 depthViewMatrix = glm::lookAtLH((cameraPositionVulkan - Direction * 10000).GetGLMVector(), center.GetGLMVector(), glm::vec3(0.0f, -1.0f, 0.0f));
	return depthProjectionMatrix * depthViewMatrix;
}

