#version 400 core
layout(location = 0) in vec3 LocalPosition;
layout(location = 1) in vec3 normal;
layout(location = 2) in int material;

uniform mat4 projection;
uniform mat4 model;// хранить позиц≥ю ≥ поворот модел≥ в глобальном св≥т≥
uniform mat4 cameraR; // матриц€ поворота камери з виставленими координатами в 0
uniform vec3 cameraP;

out vec3 Normal;
out float outMaterial;// program linker error when type int
out vec3 FragPos;  
out vec3 localPos;

out vec3 viewPos;


//----
out vec4 FragPosLightSpace;
uniform mat4 directionLightMatrix; // new
void main()
{
//    TexCoords = texCoord;
    viewPos = cameraP;

    outMaterial = float(material);
    // do in cpu side
    Normal = mat3(transpose(inverse(model))) * normal; // rotate normal by invers transpose model matrix
    
    vec4 worldPosition = model * vec4(LocalPosition, 1);
    FragPos = vec3(worldPosition);
    
    vec4 cameraPosition =  worldPosition;
    
    cameraPosition.x = worldPosition.x - cameraP.x;
    cameraPosition.y = worldPosition.y - cameraP.y;
    cameraPosition.z = worldPosition.z - cameraP.z;

    FragPosLightSpace = directionLightMatrix * vec4(FragPos, 1.0);
    
    gl_Position = projection * cameraR * cameraPosition;
};