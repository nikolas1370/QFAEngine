#version 400 core
layout(location = 0) in vec3 LocalPosition;
layout(location = 1) in vec3 normal;
layout(location = 2) in int material;
layout(location = 3) in vec3 LocalPositionNext;
layout(location = 4) in vec3 normalNext;

uniform mat4 projection;
uniform mat4 model;// хранить позиц≥ю ≥ поворот модел≥ в глобальном св≥т≥
uniform mat4 cameraR; // матриц€ поворота камери з виставленими координатами в 0
uniform vec3 cameraP;
//uniform float timeBetweenFrame;
uniform float InterTime;

out vec3 Normal;
out float outMaterial;// program linker error when type int
out vec3 FragPos;  
out vec3 localPos;

out vec3 viewPos;// new
out vec4 FragPosLightSpace;// new
uniform mat4 directionLightMatrix; // new

vec3 InterVec(float intTime, vec3 one, vec3 two)
{
    vec3 resoult = two - one;
    resoult *= intTime;
    return resoult + one;
}

void main()
{
    /*
    vec3 InterPosition = InterVec(InterTime > 100 ? 0 : 1, LocalPosition, LocalPositionNext);
    vec3 InterNormal = InterVec(InterTime > 100? 0: 1, normal, normalNext);
    */
    vec3 InterPosition = InterVec(InterTime , LocalPosition, LocalPositionNext);
    vec3 InterNormal = InterVec(InterTime, normal, normalNext);

    
    
    viewPos = cameraP;
    

    
    outMaterial = float(material); 
    FragPos = vec3(model * vec4(InterPosition, 1));
    Normal = mat3(transpose(inverse(model))) * InterNormal; // rotate normal by invers transpose model matrix
    
    vec4 worldPosition = model * vec4(InterPosition, 1);
    FragPos = vec3(worldPosition);
    
    vec4 cameraPosition =  worldPosition;
    
    cameraPosition.x = worldPosition.x - cameraP.x;
    cameraPosition.y = worldPosition.y - cameraP.y;
    cameraPosition.z = worldPosition.z - cameraP.z;
    
    FragPosLightSpace = directionLightMatrix * vec4(FragPos, 1.0);

    gl_Position = projection * cameraR * cameraPosition;
};