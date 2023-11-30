#version 330 core
in vec2 TexCoords;
out vec4 color;
uniform sampler2D glyphTexture[8];

uniform vec3 outlineColor; 
uniform vec3 textColor; 
uniform int outline;
uniform float opacity;

in float AtlasIndex;

const float smoothing = 1.0/16.0;
const float outlineWidth = 3.0/16.0; //will need to be tweaked
const float outerEdgeCenter = 0.5 - outlineWidth; //for optimizing below calculation
void main()
{       
    // https://stackoverflow.com/questions/26155614/outlining-a-font-with-a-shader-and-using-distance-field
    int index = int(round(AtlasIndex));       
    float distance = texture(glyphTexture[index], TexCoords).r;   
    if(outline == 0)
    {
        float alpha = smoothstep(0.5 - smoothing, 0.5 + smoothing, distance);
        gl_FragColor = vec4(textColor, alpha * opacity);
    }
    else
    {
        float alpha = smoothstep(outerEdgeCenter - smoothing, outerEdgeCenter + smoothing, distance);
        float border = smoothstep(0.5 - smoothing, 0.5 + smoothing, distance);        
        gl_FragColor = vec4( mix(outlineColor.rgb , textColor.rgb, border), alpha * opacity);
    }    
}  
