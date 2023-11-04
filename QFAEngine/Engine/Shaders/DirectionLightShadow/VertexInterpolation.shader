#version 400 core
layout(location = 0) in vec3 aPos;
layout(location = 3) in vec3 aPosNext;
uniform mat4 directionLightMatrix;
uniform mat4 model;
uniform float InterTime;

vec3 InterVec(float intTime, vec3 one, vec3 two)
{
	vec3 resoult = two - one; // two - one
	resoult *= intTime;

	return resoult + one;
}

void main()
{
	vec3 resPos = InterVec(InterTime, aPos, aPosNext);
	gl_Position = directionLightMatrix * model * vec4(resPos, 1.0);
}