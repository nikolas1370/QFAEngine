#pragma once	
#include <Object/Object.h>
#include <Math/Vector.h>

class QFAShaderProgram;
class QFAViewport;

class QDirectionLight : public QObject
{
	friend QFAViewport;
	unsigned int depthMapFBO;
	const unsigned int SHADOW_WIDTH = 1000, SHADOW_HEIGHT = 1000;
	unsigned int depthMap;
	unsigned int VAOSuper;

	
	FVector Direction = FVector(-1,0,0);	
	bool CastShadows = true;

	
public:
	// in private
	void StartFrame();



	QDirectionLight();
	~QDirectionLight();

	/*
		calsculate Light Matrix and set in shader
	*/
	void SetLightMatrix(const FVector& cameraPositionOpenGL, QFAShaderProgram* sp);
	void SetShadowMap(QFAShaderProgram* sp);
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

