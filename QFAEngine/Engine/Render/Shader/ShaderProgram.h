#pragma once
#include "Shader.h"
#include <Math/Math.h>
#include <Math/Vector.h>

struct Material;

class QFAShaderProgram
{ 
	static unsigned int CurentRunProgramId;
	static const int LocationGlyphTextureCount = 8;

	int LocationProjectionMatrix = -2;
	int LocationCameraRotationMatrix = -2;
	int LocationCameraPosition = -2;
	int LocationModelMatrix = -2;
	int LocationInterpolationTime = -2;	
	int LocationMaterials[101];
	int LocationDirectionShadowMap = -2;
	int LocationDirectionLigthMatrix = -2;
	int LocationShadowOn = -2;
	int LocationGlyphTexture[LocationGlyphTextureCount];
	int LocationTextColor = -2;
	int LocationOutlineColor = -2;
	int LocationOutline = -2;
	int LocationOpacity = -2;

public:
	// in private
	unsigned int ShaderProgramId;
	

	QFAShaderProgram(std::string vertex, std::string fragment, bool isPath = true, std::string geometry = "");
	~QFAShaderProgram();

	// remove no need 
	//unsigned int GetShaderProgramId() const;

	void Use();

	void SetProjectionMatrix(const glm::mat4& matrix);
	void SetCameraRotationMatrix(const glm::mat4& matrix);
	void SetCameraPosition(const FVector& position);
	void SetModelMatrix(const glm::mat4& matrix);
	void SetInterpolationTime(const float interpolationTime);
	void SetMaterials(Material* materials, int count);
	void SetDirectionShadowMap(unsigned int id, int textureSlote);
	void SetDirectionLigthMatrix(const glm::mat4& matrix);
	void SetGlyphsTestureUnit(int indexTexture);
	



	/*
		now suport only direction light

	*/
	void SetShadowOn(bool color);
	void SetTextColor(FVector indexTexture);
	void SetOutlineColor(FVector colorindexTexture);
	void SetOutlineStatus(bool outline);
	void SetOpacity(float Opacity);
};


