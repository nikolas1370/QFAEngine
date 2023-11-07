#include "DirectionLight.h"
#include <Render/Shader/ShaderProgram.h>
#include <Tools/Debug/OpenGlStuff.h>


QDirectionLight::QDirectionLight()
{
	glGenFramebuffers(1, &depthMapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	GLCall(glGenVertexArrays(1, &VAOSuper));
	GLCall(glBindVertexArray(VAOSuper));

	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);// all out shadow map be borderColor(depth value 1)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);// all out shadow map be borderColor(depth value 1)


	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

QDirectionLight::~QDirectionLight()
{
}

void QDirectionLight::StartFrame()
{
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	GLCall(glBindVertexArray(VAOSuper));
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}


void QDirectionLight::SetLightMatrix(const FVector& cameraPositionOpenGL, QFAShaderProgram* sp)
{

	float near_plane = 0.1f, far_plane = 700.5f;
	// more value more shadow camera be see
	glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);


	glm::mat4 lightView = glm::lookAt(glm::vec3(0),
		glm::vec3(Direction.Y, Direction.Z, Direction.X),
		glm::vec3(0.0f, 1.0f, 0.0f));

	lightView[3][0] = cameraPositionOpenGL.X * -1;
	lightView[3][1] = cameraPositionOpenGL.Y * -1;
	lightView[3][2] = cameraPositionOpenGL.Z * -1;


	glm::mat4 lightSpaceMatrix = lightProjection * lightView;


	sp->SetDirectionLigthMatrix(lightSpaceMatrix);
}

void QDirectionLight::SetShadowMap(QFAShaderProgram* sp)
{
	sp->SetDirectionShadowMap(depthMap, 7);
}
