#version 460 core
layout (location = 0) in vec3 vertex; 
layout (location = 1) in vec2 texCoor;

layout(set = 0,binding = 0) uniform UniformBufferObject 
{
    mat4 projection;
} prog;


layout(location = 0) out vec2 TexCoords;
void main()
{

    gl_Position = prog.projection * vec4(vertex.xyz, 1.0);
    TexCoords = texCoor;
    
}