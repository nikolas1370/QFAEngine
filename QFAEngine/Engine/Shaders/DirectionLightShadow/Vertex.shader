#version 400 core

layout(location = 0) in vec3 aPos;
uniform mat4 directionLightMatrix;
uniform mat4 model;
void main()
{
    gl_Position = directionLightMatrix * model * vec4(aPos, 1.0);
}

