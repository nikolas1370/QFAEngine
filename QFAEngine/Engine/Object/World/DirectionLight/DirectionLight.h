#pragma once	
#include <Object/Object.h>
#include <Math/Vector.h>
#include <Core/EngineStuff/EngineClass.h>
class QFAShaderProgram;
class QFAEngineViewport;
class QFAEngineWindow;

class QFAEXPORT QDirectionLight : public QObject
{
	QFAEngineClassIn(QDirectionLight)
	friend QFAEngineViewport;
	friend QFAEngineWindow;

	unsigned int depthMapFBO;
	//const unsigned int SHADOW_WIDTH = 1000, SHADOW_HEIGHT = 1000;
	unsigned int depthMap;
	unsigned int VAOSuper;

	// in vulkan coordinate
	FVector Direction = FVector(0.0f, 0.0f, 1.0f);
	FVector Ambient = FVector(0.1f);
	FVector Diffuse = FVector(1);
	FVector Specular = FVector(1);
	bool CastShadows = true;

	
public:
	QDirectionLight();
	~QDirectionLight();

	glm::mat4 GetLightMatrix();

	inline bool GetCastShadow()
	{
		return CastShadows;
	}

	inline void SetCastShadow(bool castShadow)
	{
		CastShadows = castShadow;
	}
};

