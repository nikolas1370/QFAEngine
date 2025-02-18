#version 460 core

layout(location = 0) out vec4 FragColor ;
layout(location = 1) out uint FragMeshId;

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

layout(set = 0, binding = 1) uniform sampler2D shadowMap;

layout(set = 1, binding = 1) uniform UniformBufferObject 
{// https://vkguide.dev/docs/chapter-4/storage_buffers/  for Material materials[];
     
    // samplerCube depthMap; 
     //int shadowOn;
      DirLight DL;
      Material materials[101];// 0 default
      
} prog;



layout(location = 0) in vec3 Normal;  
layout(location = 2) in vec3 FragPos;  
layout(location = 3) in vec3 localPos;






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



layout( push_constant ) uniform MeshId {
  uint id;
} meshId;




vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);



 
layout(location = 4) in vec4 FragPosLightSpace;

layout(location = 1) in float outMaterial; // program linker error when type int

float ShadowCalculation( vec3 normal, vec3 lightDir)
{


// perform perspective divide
    vec3 projCoords = FragPosLightSpace.xyz;// / FragPosLightSpace.w;
    // transform to [0,1] range
    //float z = projCoords.z;// + 0.5
    //projCoords = projCoords * 0.5 + 0.5;
    //projCoords.z = z;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;


    //return shadow;
    //float bias = 0.005; // for disable self shading // ñâìî çàò³íåííÿ
    /*
    Here we have a maximum bias of 0.05 and a minimum of 0.005 based on 
    the surface's normal and light direction. This way, surfaces like the
    floor that are almost perpendicular to the light source get a small bias,
    while surfaces like the cube's side-faces get a much larger bias.
    The following image shows the same scene but now with a shadow bias: 
    */
    
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005); // inpruf
        // perform perspective divide
    


    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    closestDepth = texture(shadowMap, projCoords.xy).r;
    // get depth of current fragment from light's perspective
    currentDepth = projCoords.z;
    
    shadow = 0.0;
    // without PCF
    shadow = currentDepth  > closestDepth ? 1.0 : 0.0;
        //return shadow;
    // with PCF
    /*------------*/
    
    
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth  > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    
    //return shadow;
    
    // blurrier shadows:
    // PCF: percentage-closer filtering 
    
    shadow = 0.0;
    texelSize = 1.0 / textureSize(shadowMap, 0);
    const int halfkernelWidth = 3;
    for (int x = -halfkernelWidth; x <= halfkernelWidth; ++x)
    {
        for (int y = -halfkernelWidth; y <= halfkernelWidth; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r   ;
            shadow += currentDepth  > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= ((halfkernelWidth * 2 + 1) * (halfkernelWidth * 2 + 1));
    
    if (projCoords.z > 1.0)
        shadow = 0.0;
    
    
    return shadow;
    
}

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{


    //int shadowOn = 1;
    //float shadow = shadowOn == 1 ? ShadowCalculation(normal, light.direction) : 0;
    float shadow =  ShadowCalculation(normal, light.direction);
    //float shadow = 0;
    
    int index = int(round(outMaterial));
    //int index = 1;
    
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 512);// 512 in material

    
    // combine results
    vec3 ambient = light.ambient * prog.materials[index].Color; 
    vec3 diffuse = light.diffuse * diff * prog.materials[index].Color * (1 - shadow); 
    vec3 specular = light.specular * spec * prog.materials[index].Specular * (1 - shadow); 
    return (ambient + diffuse + specular);
    
}


void main()
{


    FragMeshId = meshId.id;


    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(-FragPos);
       
    vec3 result = CalcDirLight(prog.DL, norm, viewDir);
          
    FragColor = vec4(result, 1.0);
}