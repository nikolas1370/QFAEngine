#version 400 core
layout(location = 0) out vec4 FragColor ;




/*not this les */
in vec2 TexCoords;
in float v_texIndex;
in vec3 Normal;  
in vec3 FragPos;  
//in vec3 lightPos;

uniform sampler2D u_Texture[16];// ned more shang number
/**/

 



in vec3 localPos;

//gCameraLocalPos  осце нада зпихнуть 
//на 21,13 зупинено те де він показує якісь міхінації з чимось якшо небуде
//робить правельно то передивись



  
/*----*/

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 

struct DirLight {
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;

};

struct SpotLight {
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

#define NR_POINT_LIGHTS 4



uniform vec3 viewPos; // OK

/*
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
*/
uniform Material material;// OK

// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);


void main()
{
    
    DirLight dirLight;
    dirLight.direction=vec3(-0.2, -1, -0.3);
    dirLight.ambient = vec3(0.05, 0.05, 0.05);
    dirLight.diffuse = vec3(0.4, 0.4, 0.4);
    dirLight.specular = vec3(0.5, 0.5, 0.5);
    
    PointLight pointLights[NR_POINT_LIGHTS];
    pointLights[0].position = vec3(3.0f, -0.0f, -23.0f);
    pointLights[0].ambient = vec3(0.05, 0.05, 0.05);
    pointLights[0].diffuse = vec3(0.8f, 0.8f, 0.8f);
    pointLights[0].specular = vec3(1.0f, 1.0f, 1.0f);
    pointLights[0].constant = 1;
    pointLights[0].linear = 0.09;
    pointLights[0].quadratic = 0.032;
    
    pointLights[1].position = vec3(0.0f, -3.0f, -10.0f);
    pointLights[1].ambient = vec3(0.05, 0.05, 0.05);
    pointLights[1].diffuse = vec3(0.8f, 0.8f, 0.8f);
    pointLights[1].specular = vec3(1.0f, 1.0f, 1.0f);
    pointLights[1].constant = 1;
    pointLights[1].linear = 0.09;
    pointLights[1].quadratic = 0.032;
    
    pointLights[2].position = vec3(-3.0f, -0.0f, -10.0f);
    pointLights[2].ambient = vec3(0.05, 0.05, 0.05);
    pointLights[2].diffuse = vec3(0.8f, 0.8f, 0.8f);
    pointLights[2].specular = vec3(1.0f, 1.0f, 1.0f);
    pointLights[2].constant = 1;
    pointLights[2].linear = 0.09;
    pointLights[2].quadratic = 0.032;
    
    pointLights[3].position = vec3(3.0f, -0.0f, -10.0f);
    pointLights[3].ambient = vec3(0.05, 0.05, 0.05);
    pointLights[3].diffuse = vec3(0.8f, 0.8f, 0.8f);
    pointLights[3].specular = vec3(1.0f, 1.0f, 1.0f);
    pointLights[3].constant = 1;
    pointLights[3].linear  = 0.09;
    pointLights[3].quadratic = 0.032;
    
    
    
    SpotLight spotLight;
    spotLight.position = viewPos;
    spotLight.direction = vec3(0, 0, -1);
    spotLight.ambient = vec3(0);
    spotLight.diffuse = vec3(1);
    spotLight.specular = vec3(1);
    spotLight.constant = 1;
    spotLight.linear = 0.09;
    spotLight.quadratic = 0.032;
    spotLight.cutOff = cos(0.2181662);
    spotLight.outerCutOff = cos(0.3054326);
    
    
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    // == =====================================================
    // Our lighting is set up in 3 phases: directional, point lights and an optional flashlight
    // For each phase, a calculate function is defined that calculates the corresponding color
    // per lamp. In the main() function we take all the calculated colors and sum them up for
    // this fragment's final color.
    // == =====================================================
    // phase 1: directional lighting
    vec3 result = CalcDirLight(dirLight, norm, viewDir);
    // phase 2: point lights
    for (int i = 0; i < NR_POINT_LIGHTS; i++)    
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    // phase 3: spot light
    result += CalcSpotLight(spotLight, norm, FragPos, viewDir);
    
    FragColor = vec4(result, 1.0);    
    //FragColor = vec4(gl_FragCoord.z);
    
}



// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    return (ambient + diffuse + specular);
}

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
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}