#version 400 core
layout(location = 0) out vec4 FragColor ;

//in vec2 TexCoords;


//in vec3 outColor;
//in float outSpecular;


struct Material
{
    vec3 Color;
    float Specular;
};



struct DirLight {
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight 
    {
    vec3 position;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;

};

struct SpotLight 
    {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;  
    float constant;
    float quadratic;  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float linear;
};

struct SpotLight
{
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;  
    float constant;
    float quadratic;  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float linear;
};

in vec3 Normal;  
in vec3 FragPos;  
//uniform sampler2D u_Texture[16];// ned more shang number
in vec3 localPos;


in vec3 viewPos;
//uniform vec3 viewPos; // OK


//uniform Material material;// OK
uniform Material materials[101]; // 0 default
uniform sampler2D shadowMap;
uniform samplerCube depthMap; // ------------------------

//uniform vec3 lightPos;


// array of offset direction for sampling
vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float ShadowCalculation(vec3 fragPos)
{
    vec3 fragToLight = fragPos - lightPos;
    float currentDepth = length(fragToLight);

    float shadow = 0.0;
    float bias = 0.15;
    int samples = 1;// 20
    float viewDistance = length(viewPos - fragPos);
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
    for (int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(depthMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= far_plane; // undo mapping [0;1]
        if (currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);
    return shadow;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);


in float outMaterial; // program linker error when type int
void main()
{
    
    vec3 lightPos = vec3(0); // uniform vec3 lightPos;
    float far_plane = 1000; // uniform float far_plane;// ------------------------
    
    float Shininess = 32;
    // OutMaterial
    
    vec3 outColor = vec3(1);
    float outSpecular = 0;
    
    bool shadows = false;
    
    
    int index = int(round(outMaterial));
    //vec3 color = texture(material.diffuse, TexCoords).rgb;
    vec3 normal = normalize(Normal);
    vec3 lightColor = vec3(1);
    // ambient
    vec3 ambient = 0.0 * lightColor;
    // diffuse
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), Shininess);
    vec3 specular = spec * vec3(materials[index].Specular);
    // calculate shadow
    float shadow = shadows ? ShadowCalculation(FragPos) : 0.0;    
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * materials[index].Color;
    
    FragColor = vec4(lighting, 1.0);

}
/*
// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance    +light.quadratic * (distance * distance));
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * texture(material.specular2, TexCoords).rgb;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}
*/