#version 460 core
#extension GL_EXT_nonuniform_qualifier : enable
 // GL_EXT_nonuniform_qualifier
layout(location = 0) in vec2 TexCoords;
layout(location = 1) in float AtlasIndex;

layout(location = 0) out vec4 color;
//uniform sampler2D glyphTexture[7];
layout(binding = 1) uniform sampler2D glyphTexture[];


layout(binding = 2) uniform UniformBufferTextParam 
{    
    vec3 textColor;     
    int outline;
    vec3 outlineColor;         
    float opacity;
} textParam;





const float smoothing = 1.0/16.0;
const float outlineWidth = 3.0/16.0; //will need to be tweaked
const float outerEdgeCenter = 0.5 - outlineWidth; //for optimizing below calculation
void main()
{     
    int qw = int(round(AtlasIndex));
// nonuniformEXT(index)
    float distance = texture(glyphTexture[qw], TexCoords).r;    
    if(textParam.outline == 0)
    {
        float alpha = smoothstep(0.5 - smoothing, 0.5 + smoothing, distance);         
        color = vec4(textParam.textColor, alpha * textParam.opacity);  
    }
    else
    {
        float alpha = smoothstep(outerEdgeCenter - smoothing, outerEdgeCenter + smoothing, distance);
        float border = smoothstep(0.5 - smoothing, 0.5 + smoothing, distance);        
        color = vec4( mix(textParam.outlineColor.rgb , textParam.textColor.rgb, border), alpha * textParam.opacity);
    }    
}  
