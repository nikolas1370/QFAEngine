#version 460 core
layout(location = 0) in vec2 TexCoords;

layout(location = 0) out vec4 color;
layout(binding = 0) uniform sampler2D imageTexture;





float near = 0.1;
float far = 1000.0;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));
}


void main()
{     

/*
    float depth = LinearizeDepth(texture(imageTexture, TexCoords).r) / far;
    color = vec4(vec3(depth), 1.0);// perspective
    */
    
      color = vec4(vec3(texture(imageTexture, TexCoords)), 1.0);// orthographic(shadow or 2d) 

}  

