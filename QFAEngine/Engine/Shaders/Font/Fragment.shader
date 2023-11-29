#version 330 core
in vec2 TexCoords;
out vec4 color;
uniform sampler2D glyphTexture[8];
//uniform vec3 textColor;

in float AtlasIndex;

void main()
{   
    
       int index = int(round(AtlasIndex));
       //int index = AtlasIndex;
        vec3 textColor = vec3(1);
    //vec4 sampled = vec4(1.0, 1.0, 1.0, texture(glyphTexture, vec2(TexCoords.x + (23.0/60.0), TexCoords.y)).r);
        vec4 sampled = vec4(1.0, 1.0, 1.0, texture(glyphTexture[index], TexCoords).r);
        color = vec4(textColor, 1.0) * sampled;
        
    
}  
