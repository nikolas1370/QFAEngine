#version 460 core
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
layout (location = 1) in int atlasIndex;


layout(set = 0,binding = 0) uniform UniformBufferObject 
{
    mat4 projection;
} prog;


layout(location = 0) out vec2 TexCoords;
layout(location = 1) out float AtlasIndex;
void main()
{


    gl_Position = prog.projection * vec4(vertex.xy, 0.0, 1.0);
    //gl_Position =  vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
    AtlasIndex = float(atlasIndex);    
}