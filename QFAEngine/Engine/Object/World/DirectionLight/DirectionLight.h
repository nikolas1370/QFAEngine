#pragma once	
#include <Object/Object.h>
#include <Math/Vector.h>

class QFAShaderProgram;
class QFAViewport;
class QFAWindow;

class QDirectionLight : public QObject
{
	friend QFAViewport;
	friend QFAWindow;
	unsigned int depthMapFBO;
	//const unsigned int SHADOW_WIDTH = 1000, SHADOW_HEIGHT = 1000;
	unsigned int depthMap;
	unsigned int VAOSuper;

	// in vulkan coordinate
	FVector Direction = FVector(0,0,1);	
	FVector Ambient = FVector(0.1);
	FVector Diffuse = FVector(1);
	FVector Specular = FVector(1);
	bool CastShadows = true;

	
public:




	QDirectionLight();
	~QDirectionLight();



	glm::mat4 GetLightMatrix(const FVector& cameraPositionVulkan);

	inline bool GetCastShadow()
	{
		return CastShadows;
	}



	inline void SetCastShadow(bool castShadow)
	{
		CastShadows = castShadow;
	}



private:

};

