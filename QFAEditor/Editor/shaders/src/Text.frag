#version 460 core
#extension GL_EXT_nonuniform_qualifier : enable
layout(location = 0) in vec2 TexCoords;
layout(location = 1) in float AtlasIndex;


layout(location = 0) out vec4 out_color;

layout(set = 1,binding = 0) uniform sampler2D glyphTexture[];

layout(set = 1, binding = 1) uniform UniformBufferTextParam 
{    
    vec3 textColor;     
    int outline;
    vec3 outlineColor;         
    float opacity;
    int overflow;
    float leftTopX;
    float leftTopY;
    float rightBottomX;
    float rightBottomY;
} textParam;


 layout(origin_upper_left) in vec4 gl_FragCoord;


const float smoothing = 1.0/16.0;
const float outlineWidth = 3.0/16.0; //will need to be tweaked
const float outerEdgeCenter = 0.5 - outlineWidth; //for optimizing below calculation
void main()
{  
    if(textParam.overflow != 0)
    {

        if( gl_FragCoord.x < textParam.leftTopX || gl_FragCoord.y < textParam.leftTopY ||
            gl_FragCoord.x > textParam.rightBottomX || gl_FragCoord.y > textParam.rightBottomY)
        {
            out_color = vec4(0);
            return;
        }        
    }

    

    int qw = int(round(AtlasIndex));

    float distance = texture(glyphTexture[qw], TexCoords).r;    
    if(textParam.outline == 0)
    {
        float alpha = smoothstep(0.5 - smoothing, 0.5 + smoothing, distance);         
        out_color = vec4(textParam.textColor, alpha * textParam.opacity);  
    }
    else
    {
        float alpha = smoothstep(outerEdgeCenter - smoothing, outerEdgeCenter + smoothing, distance);
        float border = smoothstep(0.5 - smoothing, 0.5 + smoothing, distance);        
        out_color = vec4( mix(textParam.outlineColor.rgb , textParam.textColor.rgb, border), alpha * textParam.opacity);
    }    

}
  
