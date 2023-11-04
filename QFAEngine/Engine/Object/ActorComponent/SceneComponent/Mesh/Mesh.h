#pragma once
//#include <Object/ActorComponent/SceneComponent/SceneComponent.h>
#include <Object/ActorComponent/SceneComponent/Mesh/MeshBase.h>
#include <Tools/Debug/OpenGlStuff.h>
#include <Render/Time.h>
#include <Render/Shader/ShaderProgram.h>






class QMesh : public QMeshBaseComponent
{// in destructor delete buffers
	friend Render;

	static ShaderProgram* SShaderProgram;
	static ShaderProgram* SShaderShadowProgram;
	static std::string VertexShaderPath;
	static std::string FragmentShaderPath;


	static std::string VertexShaderShaderPath;
	static std::string FragmentShaderShaderPath;

	int curentFrame = 1;
	double PlayTime = 0;// deltaTimeAcumulate
	float InterpolationTime;
	//recording is played
	bool Played = true;
	bool Repeat = true;


protected:

	inline ShaderProgram* GetShaderProgram() override
	{
		return SShaderProgram;
	}

	inline ShaderProgram* GetShadowShaderProgram() override
	{
		return SShaderShadowProgram;
	}

public:

	// in private
	MeshFrames* Mf;// rename MF

	QMesh(MeshFrames* mf);
	void Bind(bool isShadow = false) override;

	int GetIndexCount() override;

	/* calculate animation (in Bind can not, Bind Call in shadow render and mesh render) */
	void StartFrame(); // remove in private and be call in Render

	void Play();
	void Stop();
private:

};
