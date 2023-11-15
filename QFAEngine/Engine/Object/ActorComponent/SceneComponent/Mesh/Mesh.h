#pragma once
#include <Object/ActorComponent/SceneComponent/Mesh/MeshBase.h>
#include <Tools/Debug/OpenGlStuff.h>
#include <Render/Time.h>
#include <Render/Shader/ShaderProgram.h>






class QMesh : public QMeshBaseComponent
{// in destructor delete buffers
	

	static QFAShaderProgram* SShaderProgram;
	static QFAShaderProgram* SShaderShadowProgram;
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

	uint64_t StartFrameTime;
protected:

	inline QFAShaderProgram* GetShaderProgram() override
	{
		return SShaderProgram;
	}

	inline QFAShaderProgram* GetShadowShaderProgram() override
	{
		return SShaderShadowProgram;
	}

public:

	// in private
	MeshFrames* Mf;// rename MF

	QMesh(MeshFrames* mf);
	void Bind(uint64_t startFrameTime, bool isShadow ) override;

	int GetIndexCount() override;

	/* calculate animation (in Bind can not, Bind Call in shadow render and mesh render) */
	void StartFrame(); // remove in private and be call in Render

	void Play();
	void Stop();
private:

};
