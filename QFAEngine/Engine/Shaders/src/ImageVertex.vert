#version 460 core
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>



layout(binding = 0) uniform UniformBufferObject 
{
    mat4 projection;
} prog;


layout(location = 0) out vec2 TexCoords;

void main()
{
    gl_Position = vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
    
}