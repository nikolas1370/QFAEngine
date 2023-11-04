#version 400 core
layout(location = 0) out vec4 FragColor ;

//in vec2 TexCoords;



in vec3 Normal;  
in vec3 FragPos;  
//uniform sampler2D u_Texture[16];// ned more shang number
in vec3 localPos;

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

in vec3 viewPos;
//uniform vec3 viewPos; // OK
//uniform Material material;// OK
uniform sampler2D shadowMap;
uniform samplerCube depthMap; // ------------------------


uniform int shadowOn;
//uniform float far_plane;// ------------------------
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



vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
float ShadowCalculation(vec3 fragPos);


 // uniform float far_plane;// ------------------------
//before be in  uniform vec3 lightPos;
in vec4 FragPosLightSpace;
float ShadowCalculation( vec3 normal, vec3 lightDir)
{
    //float bias = 0.005; // for disable self shading // свмо затінення
    /*
    Here we have a maximum bias of 0.05 and a minimum of 0.005 based on 
    the surface's normal and light direction. This way, surfaces like the
    floor that are almost perpendicular to the light source get a small bias,
    while surfaces like the cube's side-faces get a much larger bias.
    The following image shows the same scene but now with a shadow bias: 
    */
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005); // inpruf
        // perform perspective divide
    vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5; // turm fron -1 1 to 0 1
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    
    
    // without PCF
    //float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    // with PCF
    /*
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    */
    
    // blurrier shadows:
    // PCF: percentage-closer filtering 
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    const int halfkernelWidth = 3;
    for (int x = -halfkernelWidth; x <= halfkernelWidth; ++x)
    {
        for (int y = -halfkernelWidth; y <= halfkernelWidth; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= ((halfkernelWidth * 2 + 1) * (halfkernelWidth * 2 + 1));
    
    if (projCoords.z > 1.0)
        shadow = 0.0;
    
    
    return shadow;
}
uniform Material materials[101]; // 0 default
in float outMaterial; // program linker error when type int


// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    float shadow = shadowOn == 1 ? ShadowCalculation(normal, light.direction) : 0;
    int index = int(round(outMaterial));
    
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 512);// 512 in material
    // combine results
    vec3 ambient = light.ambient * materials[index].Color; // before "outColor"
    vec3 diffuse = light.diffuse * diff * materials[index].Color * (1 - shadow); // before "outColor"
    vec3 specular = light.specular * spec * materials[index].Specular * (1 - shadow); // before "outSpecular"
    return (ambient + diffuse + specular);
    
}


void main()
{
    DirLight DL;
    DL.ambient = vec3(0.3);
    DL.diffuse = vec3(1);
    DL.specular = vec3(1);
    DL.direction = vec3(0, 0, -1);
    
    
    //vec3 lightPos = vec3(0, 0, 0);
    
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    
    /*-------------*/
    //vec3 color = texture(material.diffuse, TexCoords).rgb;
    vec3 result = CalcDirLight(DL, norm, viewDir);
        
    FragColor = vec4(result, 1.0);

}




