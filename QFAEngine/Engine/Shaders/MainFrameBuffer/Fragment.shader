#version 400 core
layout(location = 0) out vec4 color;
uniform sampler2D u_Texture;
in vec2 _texCor;
float near = 0.1;
float far = 100.0;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));
}
void main()
{
    float depth = LinearizeDepth(texture(u_Texture, _texCor).r) / far;
    //color = vec4(vec3(depth), 1.0);// perspective
    color = vec4(vec3(texture(u_Texture, _texCor).r), 1.0);// orthographic(shadow or 2d) 
};
