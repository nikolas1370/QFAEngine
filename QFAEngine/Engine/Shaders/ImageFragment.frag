#version 460 core
layout(location = 0) in vec2 TexCoords;

layout(location = 0) out vec4 color;


layout(set = 1,binding = 0) uniform sampler2D imageTexture;
layout(set = 1, binding = 1) uniform UniformBufferTextParam 
{    
    float opacity;
    int overflow;
    float leftTopX;
    float leftTopY;
    float rightBottomX;
    float rightBottomY;
} imageParam;

void main()
{     
    if(imageParam.overflow != 0)
    {

        if( gl_FragCoord.x < imageParam.leftTopX || gl_FragCoord.y < imageParam.leftTopY ||
            gl_FragCoord.x > imageParam.rightBottomX || gl_FragCoord.y > imageParam.rightBottomY)
        {
            color = vec4(0);
            return;
        }        
    }    

    
    color = vec4(vec3(texture(imageTexture, TexCoords)), imageParam.opacity);
}  

/*
float LinearizeDepth(float depth)
{
    
float near = 0.1;
float far = 1000.0;


    float z = depth * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));
}
    float depth = LinearizeDepth(texture(imageTexture, TexCoords).r) / far;
    color = vec4(vec3(depth), 1.0);// perspective
    */