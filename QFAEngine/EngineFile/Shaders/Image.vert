#version 460 core
layout (location = 0) in vec3 vertex; 
layout (location = 1) in vec2 texCoor;

layout(set = 0,binding = 0) uniform UniformBufferObject 
{
    mat4 projection;
} prog;


layout(set = 1, binding = 2) uniform UniformBufferOffset
{
    float offset;
    float offsetX;
} Offset;

layout(location = 0) out vec2 TexCoords;
void main()
{
    vec3 vert = vertex;
    vert.x += Offset.offsetX;
    vert.y += Offset.offset;

    gl_Position = prog.projection * vec4(vert.xyz, 1.0);
    TexCoords = texCoor;
    
}