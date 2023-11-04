#version 400 core 
layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texCor;
out vec2 _texCor;
void main()
{
	gl_Position = vec4(position.x, position.y, 0, 1);
	_texCor = texCor;
};