#include "pch.h"
#include "DirectionLight.h"
#include <Core/EngineStuff/VulkanSuff.h>
QFAEngineClassOut(QDirectionLight);
QDirectionLight::QDirectionLight()
{

}

QDirectionLight::~QDirectionLight()
{
}




glm::mat4 QDirectionLight::GetLightMatrix()
{
	float near_plane = 0.1f, far_plane = 1000;
	glm::mat4 depthProjectionMatrix = glm::orthoLH_ZO(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
	glm::mat4 depthViewMatrix = glm::lookAtLH((Direction * -100).GetGLMVector(), Direction.GetGLMVector(), glm::vec3(0.0f, -1.0f, 0.0f));
	return depthProjectionMatrix * depthViewMatrix;
}

