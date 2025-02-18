#version 460 core
layout (location = 0) in vec3 vertex; 
layout (location = 1) in vec2 texCord; 
layout (location = 2) in int atlasIndex;


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
layout(location = 1) out float AtlasIndex;

void main()
{
    vec3 vert = vertex;
    vert.y += Offset.offset;
    vert.x += Offset.offsetX;

    gl_Position = prog.projection * vec4(vert.xyz, 1.0);
    TexCoords = texCord;
    AtlasIndex = float(atlasIndex);    
}