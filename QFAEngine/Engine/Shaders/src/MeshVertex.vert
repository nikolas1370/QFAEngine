#version 460 core

layout(location = 0) in vec3 LocalPosition;
layout(location = 1) in vec3 normal;
layout(location = 2) in int material;


layout(set = 0, binding = 0) uniform UniformBufferObject 
{
    mat4 projection;     
    mat4 cameraR;
    mat4 directionLightMatrix; 
    vec3 cameraP;     
} prog;

layout(set = 1, binding = 0) uniform UniformBufferObject2 
{
    mat4 model;
} model;    


layout(location = 0) out vec3 Normal;
layout(location = 1) out float outMaterial;
layout(location = 2) out vec3 FragPos;  
layout(location = 3) out vec3 localPos;

layout(location = 4) out vec3 viewPos;

layout(location = 5) out vec4 FragPosLightSpace;

const mat4 biasMat = mat4( // from -1 1 to 0 1 
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.5, 0.5, 0.0, 1.0 );

void main()
{
    viewPos = prog.cameraP;

    outMaterial = float(material);
    // do in cpu side
    Normal = mat3(transpose(inverse(model.model))) * normal; // rotate normal by invers transpose model matrix
    
    vec4 worldPosition = model.model * vec4(LocalPosition, 1);
    FragPos = vec3(worldPosition);
    
    vec4 cameraPosition =  worldPosition;
    
    cameraPosition.x = worldPosition.x - prog.cameraP.x;
    cameraPosition.y = worldPosition.y - prog.cameraP.y;
    cameraPosition.z = worldPosition.z - prog.cameraP.z;


    


    gl_Position = prog.projection * prog.cameraR * cameraPosition;



    FragPosLightSpace = biasMat * prog.directionLightMatrix * worldPosition;
};