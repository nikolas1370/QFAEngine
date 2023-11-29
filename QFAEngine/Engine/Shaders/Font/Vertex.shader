#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
layout (location = 1) in int atlasIndex;
out vec2 TexCoords;
uniform mat4 projection;

out float AtlasIndex;
void main()
{
    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
    AtlasIndex = float(atlasIndex);    
}