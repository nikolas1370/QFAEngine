#pragma once

#include "OpenGlStuff.h"
class Skybox
{
	static float skyboxVertices[];
	unsigned int VBO;
public:
	Skybox();

	void Bind();
};
